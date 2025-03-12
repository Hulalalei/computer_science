#pragma once


#include <cerrno>
#include <coroutine>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <cassert>

#include <system_error>
#include <array>

#include <co_context/timer_context.hpp>
#include <co_context/co_task.hpp>
#include <co_context/awaitables.hpp>
#include <utils/expected.hpp>
#include <utils/bytes_buffer.hpp>
#include <utils/stop_source.hpp>



struct io_context : timer_context {
    int m_epfd;
    size_t m_epcount = 0;

    // 每个线程的g_instance都不同，没有初始化的g_instance都是nullptr
    static inline thread_local io_context *g_instance = nullptr;

    io_context()
        : m_epfd(convert_error(epoll_create1(0)).expect("epoll_create")) {
        g_instance = this;
    }

    void join() {
        std::array<struct epoll_event, 128> events;
        while (!is_empty()) {
            std::chrono::nanoseconds dt = duration_to_next_timer();
#if HAS_epoll_pwait2
            struct timespec timeout, *timeoutp = nullptr;
            if (dt.count() >= 0) {
                timeout.tv_sec = dt.count() / 1'000'000'000;
                timeout.tv_nsec = dt.count() % 1'000'000'000;
                timeoutp = &timeout;
            }
            int ret =
                convert_error(epoll_pwait2(m_epfd, events.data(), events.size(),
                                           timeoutp, nullptr))
                    .expect("epoll_pwait2");
#else
            int timeout_ms = -1;
            if (dt.count() >= 0) {
                timeout_ms = dt.count() / 1'000'000;
            }
            int ret =
                convert_error(epoll_pwait(m_epfd, events.data(), events.size(),
                                           timeout_ms, nullptr))
                    .expect("epoll_pwait");
#endif
            for (int i = 0; i < ret; ++i) {
                // 回调在这执行
                auto handle = std::coroutine_handle<>::from_address(events[i].data.ptr);
                handle.resume();
                -- m_epcount;
            }
        }
    }

    ~io_context() {
        close(m_epfd);
        g_instance = nullptr;
    }

    [[gnu::const]] static io_context &get() {
        assert(g_instance);
        return *g_instance;
    }

    bool is_empty() const {
        return timer_context::is_empty() && m_epcount == 0;
    }
};


struct file_descriptor {
    int m_fd = -1;

    file_descriptor() = default;

    explicit file_descriptor(int fd) : m_fd(fd) {}

    file_descriptor(file_descriptor &&that) noexcept : m_fd(that.m_fd) {
        that.m_fd = -1;
    }

    file_descriptor &operator=(file_descriptor &&that) noexcept {
        std::swap(m_fd, that.m_fd);
        return *this;
    }

    ~file_descriptor() {
        if (m_fd == -1) {
            return;
        }
        close(m_fd);
    }
};


inline std::error_category const &gai_category() {
    static struct final : std::error_category {
        char const *name() const noexcept override {
            return "getaddrinfo";
        }

        std::string message(int err) const override {
            return gai_strerror(err);
        }
    } instance;

    return instance;
}

struct address_resolver {
    struct address_ref {
        struct sockaddr *m_addr;
        socklen_t m_addrlen;
    };

    struct address {
        union {
            struct sockaddr m_addr;
            struct sockaddr_storage m_addr_storage;
        };

        socklen_t m_addrlen = sizeof(struct sockaddr_storage);

        operator address_ref() {
            return {&m_addr, m_addrlen};
        }
    };

    struct address_info {
        struct addrinfo *m_curr = nullptr;

        address_ref get_address() const {
            return {m_curr->ai_addr, m_curr->ai_addrlen};
        }

        int create_socket() const {
            return convert_error(socket(m_curr->ai_family, m_curr->ai_socktype,
                                        m_curr->ai_protocol))
                .expect("socket");
        }

        [[nodiscard]] bool next_entry() {
            m_curr = m_curr->ai_next;
            if (m_curr == nullptr) {
                return false;
            }
            return true;
        }
    };

    struct addrinfo *m_head = nullptr;

    address_info resolve(std::string const &name, std::string const &service) {
        int err = getaddrinfo(name.c_str(), service.c_str(), NULL, &m_head);
        if (err != 0) {
            auto ec = std::error_code(err, gai_category());
            throw std::system_error(ec, name + ":" + service);
        }
        return {m_head};
    }

    address_resolver() = default;

    address_resolver(address_resolver &&that) : m_head(that.m_head) {
        that.m_head = nullptr;
    }

    ~address_resolver() {
        if (m_head) {
            freeaddrinfo(m_head);
        }
    }
};

enum class async_status {
    CONCELED = -1,
    AGAIN,
    READDOWN,
};

struct async_file : file_descriptor {
    async_file() = default;

    explicit async_file(int fd) : file_descriptor(fd) {
        int flags = convert_error(fcntl(m_fd, F_GETFL)).expect("F_GETFL");
        flags |= O_NONBLOCK;
        convert_error(fcntl(m_fd, F_SETFL, flags)).expect("F_SETFL");

        struct epoll_event event;
        event.events = EPOLLET;
        event.data.ptr = nullptr;
        convert_error(
            epoll_ctl(io_context::get().m_epfd, EPOLL_CTL_ADD, m_fd, &event))
            .expect("EPOLL_CTL_ADD");
    }

    co_task<async_status> async_read(bytes_view buf, stop_source stop = {}) {
        if (stop.stop_requested()) {
            stop.clear_stop_callback();
            co_await errno_awaitable<ECANCELED>{};
            co_return async_status::CONCELED;
        }
        auto ret = convert_error<size_t>(read(m_fd, buf.data(), buf.size()));
        // 读取结束，开始解析
        if (!ret.is_error(EAGAIN)) {
            co_return async_status::READDOWN;
        }

        co_await errno_awaitable<EAGAIN>{m_fd, stop, EPOLLIN | EPOLLERR | EPOLLET | EPOLLONESHOT};
        co_return async_status::AGAIN;
    }
};

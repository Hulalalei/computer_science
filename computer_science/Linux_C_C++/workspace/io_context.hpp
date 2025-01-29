#pragma once
#include <chrono>
#include <netdb.h>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <system_error>

#include <expected.hpp>
#include <timer_context.hpp>
#include <unistd.h>


struct io_context : timer_context {
    int m_epfd;
    size_t m_epcount = 0;

    static inline thread_local io_context *g_instance = nullptr;

    io_context(): m_epfd(convert_error(epoll_create1(0)).expect("epoll_create")) { g_instance = this; }

    void join() {
        std::array<struct epoll_event, 128> events;
        while (!is_empty()) {
            std::chrono::nanoseconds dt = duration_to_next_timer();
            struct timespec timeout, *timeoutp = nullptr;
            if (dt.count() >= 0) {
                timeout.tv_sec = dt.count() / 1'000'000'000;
                timeout.tv_nsec = dt.count() % 1'000'000'000;
                timeoutp = &timeout;
            }
            int ret = convert_error(epoll_pwait2(m_epfd, events.data(), events.size(), timeoutp, nullptr))
                .expect("epoll_wait2");
            for (int i = 0; i < ret; i ++) {
                auto cb = callback<>::from_address(events[i].data.ptr);
                cb();
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

    bool is_empty() {
        return timer_context::is_empty() && m_epcount == 0;
    }
};

struct file_descriptor {
    int m_fd = -1;
    file_descriptor() = default;
    explicit file_descriptor(int fd): m_fd(fd) {}
    file_descriptor(file_descriptor &&other) noexcept: m_fd(other.m_fd) {
        other.m_fd = -1;
    }
    file_descriptor &operator=(file_descriptor &&other) noexcept {
        std::swap(m_fd, other.m_fd);
        return *this;
    }
    ~file_descriptor() {
        if (m_fd == -1) return;
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
    // 隐式类型转换，通用地址结构
    struct address {
        union {
            struct sockaddr m_addr;
            struct sockaddr_storage m_addr_storage;
        };
        socklen_t m_addrlen = sizeof(struct sockaddr_storage);
        operator address_ref() {
            return { &m_addr, m_addrlen };
        }
    };

    struct address_info {
        struct addrinfo *m_cur = nullptr;
        int create_socket() const {
            return convert_error(socket(m_cur->ai_family, m_cur->ai_socktype, m_cur->ai_protocol))
                .expect("socket");
        }
        address_ref get_address() const {
            return { m_cur->ai_addr, m_cur->ai_addrlen };
        }
        [[nodiscard]] bool next_entry() {
            m_cur = m_cur->ai_next;
            if (m_cur == nullptr) return false;
            return true;
        }
    };


    struct addrinfo *m_head = nullptr;
    address_resolver() = default;
    address_resolver(address_resolver &&other): m_head(other.m_head) {
        other.m_head = nullptr;
    }
    ~address_resolver() {
        if (m_head) freeaddrinfo(m_head);
    }
    // 该函数是处理ip地址信息的，所以应该return一个信息包
    address_info resolve(std::string const &ip_name, std::string const &net_service) {
        auto err = getaddrinfo(ip_name.c_str(), net_service.c_str(), nullptr, &m_head);
        if (err != 0) {
            auto ec = std::error_code(err, gai_category());
            throw std::system_error(ec, ip_name + ":" + net_service);
        }
        return { m_head };
    }
};

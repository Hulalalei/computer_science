#pragma once
#include "stop_source.hpp"
#include <asm-generic/socket.h>
#include <chrono>
#include <cmath>
#include <print>
#include <span>
#include <netdb.h>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <system_error>

#include <expected.hpp>
#include <timer_context.hpp>
#include <bytes_buffer.hpp>
#include <unistd.h>


struct io_context : timer_context {
    int m_epfd;
    size_t m_epcount = 0;

    static inline thread_local io_context *g_instance = nullptr;

    io_context(): m_epfd(convert_error(epoll_create1(0)).expect("epoll_create")) { g_instance = this; }

    void join() {
        std::array<struct epoll_event, 128> events;
        while (!is_empty()) {
            // 计时器 计时
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

struct async_file : file_descriptor {
    async_file() = default;

    explicit async_file(int fd): file_descriptor(fd) {
        int flags = convert_error(fcntl(m_fd, F_GETFL)).expect("F_GETFL");
        flags |= O_NONBLOCK;
        convert_error(fcntl(m_fd, F_SETFL, flags)).expect("F_SETFL");

        struct epoll_event event;
        event.events = EPOLLET;
        event.data.ptr = nullptr;
        // 在其他地方会创建一个io_context对象
        convert_error(epoll_ctl(io_context::get().m_epfd, EPOLL_CTL_ADD, m_fd, &event))
            .expect("EPOLL_CTL_ADD");
    }

    void __epoll_callback(callback<> &&resume, uint32_t events, stop_source stop) {
        // epoll_ctl
        struct epoll_event event;
        event.events = events;
        event.data.ptr = resume.get_address();
        convert_error(epoll_ctl(io_context::get().m_epfd, EPOLL_CTL_MOD, m_fd, &event))
            .expect("EPOLL_CTL_MOD");
        ++ io_context::get().m_epcount;
        stop.set_stop_callback([resume_ptr = resume.leak_address()] {
            callback<>::from_address(resume_ptr)();
        });
    }

    void async_read(std::span<char> buf, callback<expected<size_t> > cb, stop_source stop = {}) {
#if USE_LEVEL_TRIGGER
#else
        if (stop.stop_requested()) {
            stop.clear_stop_callback();
            return cb(-ECANCELED);
        }
        auto ret = convert_error<size_t>(read(m_fd, buf.data(), buf.size()));
        if (!ret.is_error(EAGAIN)) {
            stop.clear_stop_callback();
            return cb(ret);
        }

        return __epoll_callback([this, buf, cb = std::move(cb), stop]() mutable {
            return async_read(buf, std::move(cb), stop);
        }, EPOLLIN | EPOLLERR | EPOLLET | EPOLLONESHOT, stop);
#endif
    }

    void async_write(std::span<char> buf, callback<expected<size_t> > cb, stop_source stop = {}) {
#if USE_LEVEL_TRIGGER
#else
        if (stop.stop_requested()) {
            stop.clear_stop_callback();
            return cb(-ECANCELED);
        }
        auto ret = convert_error<size_t>(write(m_fd, buf.data(), buf.size()));
        if (!ret.is_error(EAGAIN)) {
            stop.clear_stop_callback();
            return cb(ret);
        }

        return __epoll_callback([this, buf, cb = std::move(cb), stop]() mutable {
            return async_write(buf, std::move(cb), stop);
        }, EPOLLOUT | EPOLLERR | EPOLLET | EPOLLONESHOT, stop);
#endif
    }

    // 先accept，再加入到epoll中
    void async_accept(address_resolver::address &addr, callback<expected<int> > cb, stop_source stop = {}) {
#if USE_LEVEL_TRIGGER
#else
        if (stop.stop_requested()) {
            stop.clear_stop_callback();
            return cb(-ECANCELED);
        }
        auto ret = convert_error<int>(accept(m_fd, &addr.m_addr, &addr.m_addrlen));
        if (!ret.is_error(EAGAIN)) {
            stop.clear_stop_callback();
            return cb(ret);
        }

        return __epoll_callback([this, &addr, cb = std::move(cb), stop]() mutable {
            return async_accept(addr, std::move(cb), stop);
        }, EPOLLIN | EPOLLERR | EPOLLET | EPOLLONESHOT, stop);
#endif
    }

    void async_connect(address_resolver::address_info const &addr, callback<expected<int> > cb, stop_source stop = {}) {
        if (stop.stop_requested()) {
            stop.clear_stop_callback();
            return cb(-ECANCELED);
        }
        auto addr_ptr = addr.get_address();
        auto ret = convert_error(connect(m_fd, addr_ptr.m_addr, addr_ptr.m_addrlen));
        if (!ret.is_error(EINPROGRESS)) {
            stop.clear_stop_callback();
            return cb(ret);
        }

        return __epoll_callback([this, cb = std::move(cb), stop]() mutable {
            if (stop.stop_requested()) { 
                stop.clear_stop_callback();
                return cb(-ECANCELED);
            }
            int ret = 0;
            socklen_t ret_len = sizeof(ret);
            convert_error(getsockopt(m_fd, SOL_SOCKET, SO_ERROR, &ret, &ret_len))
                .expect("getsockopt");

            if (ret > 0) { ret = -ret; }
            stop.clear_stop_callback();
            return cb(ret);
        }, EPOLLOUT | EPOLLERR | EPOLLONESHOT, stop);
    }

    static async_file async_bind(address_resolver::address_info const &addr) {
        auto sock = async_file{addr.create_socket()};
        auto serv_addr = addr.get_address();
        int on = 1;
        setsockopt(sock.m_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on);
        setsockopt(sock.m_fd, SOL_SOCKET, SO_REUSEPORT, &on, sizeof on);
        convert_error(bind(sock.m_fd, serv_addr.m_addr, serv_addr.m_addrlen)).expect("bind");
        convert_error(listen(sock.m_fd, SOMAXCONN)).expect("listen");
        return sock;
    }

    async_file(async_file &&) = default;
    async_file &operator=(async_file &&) = default;
    ~async_file() {
        if (m_fd == -1) return;
        epoll_ctl(io_context::get().m_epfd, EPOLL_CTL_DEL, m_fd, nullptr);
    }
    explicit operator bool() const noexcept {
        return m_fd != -1;
    }
};

#pragma once


#include <coroutine>
#include <cerrno>
#include <fmt/core.h>
#include <fmt/format.h>
#include <co_context/io_context.hpp>
#include <utils/stop_source.hpp>


template <auto m_errno> struct errno_awaitable {
    bool await_ready() const noexcept {
        return m_errno != ECANCELED;
    }
    // EAGAIN
    std::coroutine_handle<> await_suspend(std::coroutine_handle<> coroutine) const noexcept { 
        if (m_errno == EAGAIN) {
            struct epoll_event event;
            event.events = m_events;
            event.data.ptr = coroutine.address();
            convert_error(
                epoll_ctl(io_context::get().m_epfd, EPOLL_CTL_MOD, m_fd, &event))
                .expect("EPOLL_CTL_MOD");
            ++io_context::get().m_epcount;
            m_stop.set_stop_callback(coroutine);
        } else {
            fmt::print("unknown error\n");
        }
        return coroutine;
    }
    // ECANCELED
    void await_resume() const noexcept { 
        fmt::print("读取错误: {}, 放弃连接", strerror(ECANCELED));
        return;
    }
    int m_fd;
    stop_source m_stop;
    uint32_t m_events;
};

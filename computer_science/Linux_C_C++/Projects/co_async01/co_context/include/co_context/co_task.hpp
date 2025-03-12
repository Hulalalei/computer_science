#pragma once

#include <coroutine>
#include <debug.hpp>



template <class T = int> struct co_task {
    template <class TP> struct __promise {
        std::suspend_always initial_suspend() { debug(), "init suspend"; return {}; }
        std::suspend_always final_suspend() noexcept { debug(), "final suspend"; return {}; }
        void unhandled_exception() { throw; }
        std::suspend_always yield_value(TP ret) { m_val = ret; return {}; }
        // void return_void() { m_val = 0; }
        void return_value(TP ret) { m_val = ret; }
        std::coroutine_handle<__promise> get_return_object() { debug(), "get suspend"; return std::coroutine_handle<__promise>::from_promise(*this); }

        TP m_val;
    };
    using promise_type = __promise<T>;

    std::coroutine_handle<promise_type> m_coroutine;

    co_task(std::coroutine_handle<promise_type> coroutine): m_coroutine(coroutine) { debug(), "construct"; }
    co_task(co_task &&) = delete;
    ~co_task() { m_coroutine.destroy(); }

    struct __awaiter {
        bool await_ready() const noexcept { return false; }
        std::coroutine_handle<> await_suspend(std::coroutine_handle<> coroutine) const noexcept { debug(), "after ready, begin to process"; return coroutine; }
        T await_resume() const noexcept { debug(), "resume"; return 10; }

        std::coroutine_handle<> m_coroutine;
    };
    __awaiter operator co_await() const noexcept { return __awaiter{m_coroutine}; }
    operator std::coroutine_handle<>() const noexcept { return m_coroutine; }
};

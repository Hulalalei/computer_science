#ifndef PROMISE
#define PROMISE

#include <coroutine>
#include <type_traits>



namespace co_async {
    struct Promise {
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void unhandled_exception() { throw; }
        std::suspend_always yield_value(int ret) { m_val = ret; return {}; }
        void return_value(int ret) { m_val = ret; }
        std::coroutine_handle<Promise> get_return_object() { return std::coroutine_handle<Promise>::from_promise(*this); }

        int m_val;
    };
    struct task {
        using promise_type = Promise;
        std::coroutine_handle<promise_type> m_co;
        task(std::coroutine_handle<promise_type> coroutine): m_co(coroutine) {}
        task(task &&) = delete;
        ~task() { m_co.destroy(); }

        bool await_ready() const noexcept { return false; }
        void await_suspend(std::coroutine_handle<> coroutine) const noexcept { }
        void await_resume() const noexcept { return; }
    };
}
#endif

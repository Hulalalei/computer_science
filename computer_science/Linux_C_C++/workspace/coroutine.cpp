#include <iostream>
#include <coroutine>
#include <unistd.h>
#include <debug.hpp>

// std中suspend_never和suspend_always的定义
// 用于指定协程是否总是准备就绪: 等待下回resume()才执行或者马上执行
// awaiter(原始指针) / awaitable(operator->)
// struct Suspend_never {
//     constexpr bool await_ready() const noexcept { return true; }
//
//     constexpr void await_suspend(std::coroutine_handle<> coroutine) const noexcept {}
//
//     constexpr void await_resume() const noexcept {}
// };
// struct Suspend_always {
//     constexpr bool await_ready() const noexcept { return false; }
//
//     constexpr void await_suspend(std::coroutine_handle<> coroutine) const noexcept {}
//
//     constexpr void await_resume() const noexcept {}
// };

struct Promise {
    std::suspend_always initial_suspend() { return {}; }

    std::suspend_always final_suspend() noexcept { return {}; }

    void unhandled_exception() {}
    
    std::suspend_always yield_value(int ret) {                        // ret 作为co_yield的返回值
        m_value = ret;
        return {};
    }     

    void return_value(int ret) { m_value = ret; }                // ret 作为co_return的返回值
    
    std::coroutine_handle<Promise> get_return_object() { return std::coroutine_handle<Promise>::from_promise(*this); }


    int m_value;
};

struct Task {
    using promise_type = Promise;
    Task(std::coroutine_handle<promise_type> coroutine): m_coroutine(coroutine) {}
    std::coroutine_handle<promise_type> m_coroutine;

    constexpr bool await_ready() const noexcept { return true; }

    constexpr void await_suspend(std::coroutine_handle<> coroutine) const noexcept {}

    constexpr void await_resume() const noexcept {}
};

Task hello() {
    debug(), "await before";
    co_await hello();
    debug(), "await end";
    debug(), "hello";
    co_return 0;
}


int main(int argc, char **argv) {
    debug(), "main前";
    Task t = hello();
    debug(), "main后";
    while (!t.m_coroutine.done()) {
        t.m_coroutine.resume();
        debug(), "value = ", t.m_coroutine.promise().m_value;
    }
    return 0;
}

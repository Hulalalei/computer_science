#include <coroutine>
#include <debug.hpp>
#include <enum_magic/enum_magic.hpp>




// usage: 
//
// #include <co_task/co_task.hpp>
// #include <debug.hpp>
//
// co_task::co_task<int> world() {
//     debug(), "world";
//     co_return 02;
// }
//
// co_task::co_task<int> hello() {
//     debug(), "hello";
//     debug(), co_await world();
//     debug(), "yield";
//     co_yield 03;
//     debug(), "after yield";
//     co_return 42;
// }
//
// int main() {
//     // get_return_object()获取的对象，进行初始构造
//     co_task::co_task t = hello();
//     debug(), "yield value: ", t.m_coroutine.promise().m_val;
//     debug(), "resuming";
//     t.m_coroutine.resume();
//     debug(), "return value: ", t.m_coroutine.promise().m_val;
// }


namespace co_task {
struct previous_awaiter {
    std::coroutine_handle<> m_previous;

    bool await_ready() const noexcept {
        return false;
    }

    std::coroutine_handle<>
    await_suspend(std::coroutine_handle<> coroutine) const noexcept {
        return m_previous;
    }

    void await_resume() const noexcept {}
};

template <class T> struct co_task {
    template <class TP> struct __promise {
        // suspend_never && suspend_always都是awaitable对象
        // 此协程被初始化时，是否一直被挂起
        std::suspend_always initial_suspend() { debug(), "init suspend"; return {}; }
        // 此协程即将结束时co_return，需要的处理
        // 可以选择将CPU权交到调用者那，和await_suspend类似
        auto final_suspend() noexcept { 
            debug(), "final suspend"; 
            return previous_awaiter(m_previous); 
        }
        void unhandled_exception() { throw; }
        // co_yield和co_return的返回值
        // co_yield必须resume来唤醒
        // co_return调用final_suspend
        std::suspend_always yield_value(TP ret) { m_val = ret; return {}; }
        // void return_void() { m_val = 0; }
        void return_value(TP ret) { m_val = ret; }
        std::coroutine_handle<__promise> get_return_object() { debug(), "get suspend"; return std::coroutine_handle<__promise>::from_promise(*this); }

        TP m_val;
        std::coroutine_handle<> m_previous = std::noop_coroutine();
    };
    using promise_type = __promise<T>;

    std::coroutine_handle<promise_type> m_coroutine;

    // get_return_object()返回一个coroutine_handle赋值给构造函数
    co_task(std::coroutine_handle<promise_type> coroutine): m_coroutine(coroutine) { debug(), "construct"; }
    // 移动构造删除，避免资源重复释放
    co_task(co_task &&) = delete;
    ~co_task() { m_coroutine.destroy(); }

    // co_await awiatable; 只会调用await_suspend || await_resume二者之一
    struct __awaiter {
        // 只要包含这三个成员函数，就是awaiter/awaitable
        // 是否准备充分，充分后执行await_suspend，否则执行resume;
        bool await_ready() const noexcept { return false; }
        // 挂起后，CPU权该返回到何处: 此协程 || main
        // 如果返回到其他协程，则会再调用自身协程await_resume
        // 如果返回到自身协程，则不会调用await_resume
        std::coroutine_handle<> await_suspend(std::coroutine_handle<> coroutine) const noexcept { 
            debug(), "after ready, begin to process"; 
            m_coroutine.promise().m_previous = coroutine;
            return m_coroutine; 
        }
        // co_await此对象后的返回值
        T await_resume() const noexcept { debug(), "await_resume"; return T{}; }

        std::coroutine_handle<promise_type> m_coroutine;
    };
    __awaiter operator co_await() const noexcept { return __awaiter{m_coroutine}; }
    operator std::coroutine_handle<>() const noexcept { return m_coroutine; }
};
}

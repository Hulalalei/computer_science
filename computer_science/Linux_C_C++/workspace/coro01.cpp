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
struct Awaiter {
    bool await_ready() const noexcept { return false; }

    std::coroutine_handle<> await_suspend(std::coroutine_handle<> coroutine) const noexcept {
        // if (coroutine.done())
            return std::noop_coroutine();
        // else 
        //     return coroutine;
    }

    void await_resume() const noexcept {}
};


struct Awaitable {
    Awaiter operator co_await() noexcept {
        return Awaiter();
    }
};

struct PreviousAwaiter {
    std::coroutine_handle<> m_pre = nullptr;

    bool await_ready() const noexcept { return false; }

    std::coroutine_handle<> await_suspend(std::coroutine_handle<> coroutine) const noexcept {
        if (m_pre)
            return m_pre;
        else
            return std::noop_coroutine();
    }

    void await_resume() const noexcept {}
};

template <class T>
struct Promise {
    // 协程函数最开始是否挂起
    std::suspend_always initial_suspend() { return {}; }
    // 协程函数结束时，是否挂起
    PreviousAwaiter final_suspend() noexcept { return {m_pre}; }
    void unhandled_exception() {}
    
    // 返回的always，表示执行ready后便执行suspend了，如果suspend返回void，则表示resume直接结束了
    // 如果返回coroutine，则递归执行
    // 唤醒后是否一直执行
    std::suspend_always yield_value(T ret) {                        // ret 作为co_yield的返回值
        m_value = ret;
        return {};
    }
    // void return_void();
    void return_value(T ret) { m_value = 0; }                // ret 作为co_return的返回值
    std::coroutine_handle<Promise> get_return_object() { return std::coroutine_handle<Promise>::from_promise(*this); }
    int m_value;
    std::coroutine_handle<> m_pre = nullptr;
};


// Task中封装了coroutine 和 promise_type，promise_type控制执行
// 随后task进入eventloop
template <class T>
struct Task_hello {
    using promise_type = Promise<T>;
    Task_hello(std::coroutine_handle<promise_type> coroutine): m_co(coroutine) {}
    std::coroutine_handle<promise_type> m_co;


    bool await_ready() const noexcept { return false; }
    std::coroutine_handle<> await_suspend(std::coroutine_handle<> coroutine) const noexcept {
        // if (!coroutine.done())
        //     return coroutine;
        // else
            return std::noop_coroutine();
    }
    // co_await的返回值
    int await_resume() const noexcept {
        return 42;
    }
};

template <class T>
struct Task_world {
    // 被调用时，具体的行动: await_ready(), await_suspend(), await_resume()
    using promise_type = Promise<T>;
    Task_world(std::coroutine_handle<promise_type> coroutine): m_co(coroutine) {}
    std::coroutine_handle<promise_type> m_co;

    Task_world(Task_world &&) = delete;
    ~Task_world() { m_co.destroy(); }

    bool await_ready() const noexcept { return true; }
    // 自己task里的await_ready()等表示被await时的动作
    // 形参表示调用自己的那个协程
    // co_await之前，先执行此, 再return m_co.resume()
    std::coroutine_handle<> await_suspend(std::coroutine_handle<> coroutine) const noexcept {
        m_co.promise().m_pre = coroutine;
        return m_co;
    }
    // co_await的返回值, 可以用int接收
    T await_resume() const noexcept { return m_co.promise().m_value; }
};

Task_world<int> world() {
    debug(), "await before2";
    co_yield 444;
    debug(), "hello 444";
    debug(), "await end2";
    co_return 1;
}

Task_hello<int> hello() {
    debug(), "await before";
    Task_world task = world();
    int i = co_await task;
    debug(), "in world, i: ", i;
    debug(), task.m_co.promise().m_value;
    // co_await task;
    // debug(), task.m_co.promise().m_value;
    co_yield 2;
    debug(), "hello 2";
    co_yield 4;
    debug(), "hello 4";
    co_yield 6;
    debug(), "hello 6";
    // co_await world();
    debug(), "await end";
    co_return 1;
}


int main(int argc, char **argv) {
    debug(), "main前";
    Task_hello t = hello();
    debug(), "main后";
    // t.m_co.resume();
    while (!t.m_co.done()) {
        t.m_co.resume();
        debug(), "resume";
        debug(), "value =", t.m_co.promise().m_value;
    }
    return 0;
}

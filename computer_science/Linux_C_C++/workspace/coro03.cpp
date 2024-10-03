#include <debug.hpp>
#include <coroutine>
#include <format>
#include <span>
#include <array>
#include <liburing.h>


struct Promise {
    // 控制main中协程函数逻辑
    // 返回值也是可改变的: always or never
    std::suspend_always initial_suspend() { return {}; }
    std::suspend_always final_suspend() noexcept { return {}; }
    void unhandled_exception() { throw; }
    // 决定是否让出CPU权: always or never
    std::suspend_always yield_value(int ret) { m_val = ret; return {}; }
    // void return_void() { m_val = 0; }
    void return_value(int ret) { m_val = ret; }
    std::coroutine_handle<Promise> get_return_object() { return std::coroutine_handle<Promise>::from_promise(*this); }

    int m_val;
};

struct hello_task {
    using promise_type = Promise;
    std::coroutine_handle<promise_type> m_co;
    hello_task(std::coroutine_handle<promise_type> coroutine): m_co(coroutine) {}
    hello_task(hello_task &&) = delete;
    ~hello_task() { m_co.destroy(); }

    bool await_ready() const noexcept { return false; }
    void await_suspend(std::coroutine_handle<> coroutine) const noexcept { }
    int await_resume() const noexcept { return 10; }
};

// awaiter: await_ready(), await_suspend(), await_resume()
struct world_task {
    using promise_type = Promise;
    std::coroutine_handle<promise_type> m_co;
    world_task(std::coroutine_handle<promise_type> coroutine): m_co(coroutine) {}
    world_task(world_task &&) = delete;
    ~world_task() { m_co.destroy(); }

    // 控制协程调用本协程时的逻辑
    // 如果是true，则执行resume函数，否则是suspend函数
    bool await_ready() const noexcept { return false; }
    // coroutine参数为调用者协程
    std::coroutine_handle<> await_suspend(std::coroutine_handle<> coroutine) const noexcept { 
        debug(), "world_suspend"; 
        // if (!coroutine.done()) return coroutine;
        return std::noop_coroutine();
    }
    int await_resume() { debug(), "world_resume"; m_co.resume(); return 10; }
};


struct timeout_conn {
    using promise_type = Promise;

    bool await_ready() const noexcept { return timeout; }
    void await_suspend(std::coroutine_handle<> coroutine) const noexcept {}
    int await_resume() const noexcept { 
        std::cout << "awake";
        return 0;
    }

    timeout_conn operator co_await() { return *this; }
    bool timeout;
};

world_task world() {
    debug(), "world!";
    debug(), "before world_yield";
    debug(), "after world_yield";
    co_return 444;
}

hello_task hello() {
    // co_yield会返回出值，并在此交出CPU权
    debug(), "before co_yield";
    co_yield 222;
    debug(), "after co_yield";
    debug(), "hello ";
    while (true) {
        co_await timeout_conn(false);
    }
    // world_task wt = world();
    // co_await会交出CPU权，并判断是否ready；转去执行对应函数，如果再resume一次，则为ready状态
    // debug(), "co_await before";
    // int i = co_await wt;
    // debug(), "hello获取world的co_await值：", i;
    // wt.m_co.resume();
    // debug(), "hello获取world的co_yield值：", wt.m_co.promise().m_val;
    // co_return标志协程函数结束
    co_return 333;
}



int main (int argc, char **argv) {
    debug(), "构造hello前";
    hello_task t1 = hello();
    debug(), "构造hello后";

    t1.m_co.resume();
    debug(), "yield value: ", t1.m_co.promise().m_val;
    while (!t1.m_co.done()) {
        // debug(), "loop";
        t1.m_co.resume();
        // debug(), "yield value: ", t1.m_co.promise().m_val;
    }
    
    
    return 0;
}

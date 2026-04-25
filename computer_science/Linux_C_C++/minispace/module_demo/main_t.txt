import co_content;


#include <iostream>
#include <coroutine>
#include <debug.hpp>

co_task<int> world() {
    debug(), "world";
    co_return 02;
}

co_task<int> hello() {
    debug(), "hello";
    debug(), "co_await world(): ", co_await world();
    debug(), "yield";
    co_yield 03;
    debug(), "after yield";
    co_return 42;
}

int main() {
    // get_return_object()获取的对象，进行初始构造
    co_task t = hello();
    debug(), "resuming";
    t.m_coroutine.resume();
    debug(), "after first resume, yield value01: ", t.m_coroutine.promise().m_val;
    debug(), "resuming again";
    t.m_coroutine.resume();
    debug(), "after second resume, yield value02: ", t.m_coroutine.promise().m_val;
    t.m_coroutine.resume();
    debug(), "after third resume, return value03: ", t.m_coroutine.promise().m_val;

    return 0;
}

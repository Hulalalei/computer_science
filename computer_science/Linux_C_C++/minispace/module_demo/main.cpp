import co_content;
import math;


#include <iostream>
#include <coroutine>
#include <debug.hpp>


co_task<int> world() {
    debug(), "world";
    co_return 02;
}

co_task<int> hello() {
    debug(), "hello";
    debug(), co_await world();
    debug(), "yield";
    co_yield 03;
    debug(), "after yield";
    co_return 42;
}

int main() {
    // get_return_object()获取的对象，进行初始构造
    co_task t = hello();
    debug(), "yield value: ", t.m_coroutine.promise().m_val;
    debug(), "resuming";
    t.m_coroutine.resume();
    debug(), "return value: ", t.m_coroutine.promise().m_val;
    std::cout << add(1, 2) << std::endl;
    std::cout << circle_area(6.50) << std::endl;

    return 0;
}

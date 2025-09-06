#include <chrono>

#include <co_content/co_task.hpp>
#include <coroutine>
#include <debug.hpp>
#include <thread>

template <class T>
struct echo {
    bool await_ready() const noexcept { return m_flag; }
    std::coroutine_handle<> await_suspend(std::coroutine_handle<> coroutine) const noexcept { 
        return coroutine;
    }
    T await_resume() const noexcept { 
        debug(), "await_resume"; return T{}; 
    }
    echo(T buf): m_buf(buf), m_flag(true) {}
    std::string m_buf;
    bool m_flag;
};

co_task<int> echo_chat() {
    co_await echo{"hello"};
    debug(), "please insert content: ";
    // std::string buf;
    // std::cin >> buf;
    co_return {};
}

int main(void) {
    auto t = echo_chat();
    t.m_coroutine.resume();
}

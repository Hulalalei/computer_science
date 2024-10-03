#include <coroutine>
#include <iostream>
#include <thread>
#include <chrono>

struct awaiter {
    bool await_ready() const noexcept { return false; }
    void await_suspend(std::coroutine_handle<> h) const noexcept {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    void await_resume() const noexcept { std::cout << "resume\n"; }
};

struct printer {
    struct promise_type {
        printer get_return_object() {
            return printer{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_never initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() { std::terminate(); }
    };

    std::coroutine_handle<promise_type> handle;

    ~printer() {
        if (handle) handle.destroy();
    }

    void start() {
        handle.resume();
    }
};

printer co_print_numbers() {
    for (int i = 1; i <= 5; ++i) {
        std::cout << i << std::endl;
        co_await awaiter{};
    }
    co_return;
}

int main() {
    printer p = co_print_numbers();
    p.start();
    return 0;
}

#include <iostream>
#include <string>
#include <cstdio>
#include <cstring>
#include <memory>
#include <thread>
#include <functional>
#include <atomic>
#include "lock_free_stack.hpp"

int main() {
    lock_free_stack<int> stack;
    std::thread t1([&]() {
        for (int i = 0; i < 100; ++i) {
            stack.push(i);
        }
    });
    std::thread t2([&]() {
        for (int i = 0; i < 100; ++i) {
            stack.push(i);
        }
    }
    );
    t1.join();
    t2.join();
    while (auto res = stack.pop()) {
        std::cout << *res << std::endl;
    }
    return 0;
}

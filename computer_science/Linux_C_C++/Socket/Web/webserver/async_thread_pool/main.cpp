#include <iostream>
#include "thread_pool.hpp"
#include <unistd.h>


void test() { std::cout << "hhh\n"; }

int main(int argc, char **argv) {
    thread_pool pool;
    int a = 0, b = 1;
    pool.submit([&a, &b]() { std::cout << "hello world\n"; });
    sleep(3);
    return 0;
}

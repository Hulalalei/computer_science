#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP
#endif

#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <functional>

#include "../lock_free_queue/lock_free_queue.hpp"



class thread_pool {
private:
    std::vector<std::thread> threads;
    lock_free_queue<std::function<void()>> work_queue;
    std::atomic<bool> done;
    void worker_thread();
public:
    thread_pool();
    ~thread_pool();
    void submit(std::function<void()> f);
};

thread_pool::thread_pool(): done(false) {
    for (int j = 0; j < std::thread::hardware_concurrency(); j ++) {
        threads.push_back(std::thread(&thread_pool::worker_thread, this));
    }
}

void thread_pool::worker_thread() {
    while (!done) {
        if (auto task = work_queue.pop()) (*task)();
    }
}

void thread_pool::submit(std::function<void()> f) {
    work_queue.push(f);
}

thread_pool::~thread_pool() {
    done = true;

    for (int j = 0; j < threads.size(); j ++) {
        if (threads[j].joinable()) {
            threads[j].join();
        }
    }
}
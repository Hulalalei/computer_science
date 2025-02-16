#ifndef THREAD_POOL
#define THREAD_POOL

#include <print>
#include <thread>
#include <vector>

#include <lock_free_queue.hpp>
#include <minilog.hpp>


// usage:
// thread::thread_pool<std::function<void()> > pool;
// pool.submit(speak);

namespace thread {
    template <class T>
    class thread_pool {
    public:
        thread_pool();
        void work();
        void submit(T data);
        ~thread_pool();
    private:
        std::atomic<bool> done;
        lockfree::lock_free_queue<T> task_queue;
        std::vector<std::thread> worker;
    };


    template <class T>
    thread_pool<T>::thread_pool(): done(false) {
        const int work_cnt = std::thread::hardware_concurrency();

        try {
            for (int i = 0; i < work_cnt; ++ i) {
                worker.push_back(std::thread(&thread_pool::work, this));
            }
        }
        catch(...) {
            done = true;
            minilog::log_fatal("unexpected error!"); 
            throw;
        }
    }


    template <class T>
    void thread_pool<T>::work() {
        while (!done) {
            auto opt = task_queue.pop();
            if (opt.has_value()) {
                minilog::log_trace("take away a task");
                opt.value()();
            }
            else std::this_thread::yield();
        }
    }


    template <class T>
    void thread_pool<T>::submit(T data) {
        task_queue.push(data);
    }


    template <class T>
    thread_pool<T>::~thread_pool() {
        done = true;
        for (int i = 0; i < worker.size(); i ++) {
            if (worker[i].joinable())
                worker[i].join();
        }
    }
}

#endif // !THREAD_POOL

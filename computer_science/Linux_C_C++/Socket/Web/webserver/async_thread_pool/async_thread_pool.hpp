#ifndef ASYNC_THREAD_POOL_HPP
#define ASYNC_THREAD_POOL_HPP


#include <iostream>
#include <memory>
#include <vector>
#include <thread>
#include <future>
#include <atomic>
#include <type_traits>
#include "../threadsafe_queue/threadsafe_queue.hpp"
#include "../log/log.hpp"


namespace web {
class function_wrapper {
private:
    struct impl_base {
        virtual void call() = 0;
        virtual ~impl_base() {} 
    };
    std::shared_ptr<impl_base> impl;

    template <typename F>
    struct impl_type: impl_base {
        F f;
        impl_type(F&& f_): f(std::move(f_)) {}
        void call() { f(); }
    };
    
public:
    template <typename F>
    function_wrapper(F&& f): impl(new impl_type<F>(std::move(f))) {}

    void operator()() { impl->call(); }
    function_wrapper() = default;
    function_wrapper(function_wrapper&& other): impl(std::move(other.impl)) {}
    function_wrapper& operator=(function_wrapper&& other) {
        impl = std::move(other.impl);
        return *this;
    }
    function_wrapper(const function_wrapper&) = default;
    function_wrapper(function_wrapper&) = default;
    function_wrapper& operator=(const function_wrapper&) = default;
};

class join_threads {
private:
    std::vector<std::thread>& threads;
public:
    explicit join_threads(std::vector<std::thread>& threads_): threads(threads_) {}
    ~join_threads() {
        for (unsigned long i = 0; i < threads.size(); ++ i) {
            if (threads[i].joinable())
                threads[i].join();
        }
    }
};

class async_thread_pool {
private:
    std::atomic_bool done;
    threadsafe_queue<function_wrapper> work_queue;
    std::vector<std::thread> threads;
    join_threads joiner;
    class log log_writer;

    void worker_thread() {
        while (!done) {
            function_wrapper task;
            if (work_queue.try_pop(task)) {
                log_writer.write(log::debug, "get a http_connection");
                task();
            }
            else {
                std::this_thread::yield();
            }
        }
    }
public:
    async_thread_pool(): done(false), joiner(threads) {
       unsigned const thread_count = std::thread::hardware_concurrency();
       try {
            for (unsigned i = 0; i < thread_count; ++ i) {
                threads.push_back(std::thread(&async_thread_pool::worker_thread, this));
            }
       }
       catch (...) {
            done = true;
            throw;
       }
    }

   ~async_thread_pool() { done = true; }


    template <typename FunctionType>
    std::future<typename std::result_of<FunctionType()>::type> submit(FunctionType f) {
        typedef typename std::result_of<FunctionType()>::type result_type;
        std::packaged_task<result_type()> task(std::move(f));
        std::future<result_type> res(task.get_future());
        work_queue.push(std::move(task));
        log_writer.write(log::debug, "submit a http_connection");
        return res;
    }
};

}
#endif
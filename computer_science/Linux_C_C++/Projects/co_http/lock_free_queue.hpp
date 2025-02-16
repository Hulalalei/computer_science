#ifndef LOCK_FREE_QUEUE
#define LOCK_FREE_QUEUE

#include <optional>
#include <atomic>
#include <cassert>
#include "minilog.hpp"
#include "memory_pool.hpp"



std::atomic<int> construct_cnt;
std::atomic<int> destruct_cnt;
std::atomic<int> task_cnt;
namespace lockfree {
    // true 为release，false 为dubug
    template <class T, bool Mod = false, class Allocater = memory::default_alloc>
    class lock_free_queue {
    private:
        struct node;
        // 外部计数器
        struct counted_node_ptr {
            int external_count;
            node *ptr;
        };
        // 内部计数器，存储在一个机器字32bits中，原子操作更有机会以无锁方式实现
        struct node_counter {
            unsigned internal_count : 30;
            unsigned external_counters : 2;
        };
        // 数据节点
        struct node {
            std::atomic<T*> data;
            std::atomic<node_counter> count;
            std::atomic<counted_node_ptr> next;

            node();
            void release_ref();
        };
    private:
        std::atomic<counted_node_ptr> head, tail;
        [[maybe_unused]] static Allocater alloc;
    private:
        // 计数器和tail同时改变应该是一个原子操作
        // 增加外部计数器
        static void increase_external_count(std::atomic<counted_node_ptr> &counter, counted_node_ptr &old_counter);
        // 减少内部external_counters
        static void free_external_counter(counted_node_ptr &old_node_ptr);
        // 设置新的队尾
        void set_new_tail(counted_node_ptr &old_tail, counted_node_ptr const& new_tail);
    public:
        lock_free_queue();
        void push(T data);
        [[nodiscard]] std::optional<T> pop();
        lock_free_queue &operator=(lock_free_queue &&) = delete;
        ~lock_free_queue();
    }; 


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  此处next为{0}，可以用作compare_exchange
    template <class T, bool Mod, class Allocater>
    lock_free_queue<T, Mod, Allocater>::node::node(): data(nullptr), count({0, 2}), next({0, nullptr}) {}
    

    template <class T, bool Mod, class Allocater>
    lock_free_queue<T, Mod, Allocater>::lock_free_queue() {
        // 外部计数器是两份拷贝，用于push&push或pop&pop计数 
        // 内部计数器是对于push&pop竞争的node计数
        counted_node_ptr vir_node{1, alloc.template new_obj<node>()}; 
        // counted_node_ptr vir_node{1, new node{}}; 
        ++ construct_cnt;
        head.store(vir_node, std::memory_order_relaxed);
        tail.store(vir_node, std::memory_order_relaxed);
    }


    template <class T, bool Mod, class Allocater>
    void lock_free_queue<T, Mod, Allocater>::push(T data) {
        T *new_data = alloc.template new_obj<T>(data);
        // T *new_data =  new T(data);
        
        counted_node_ptr vir_node{1, alloc.template new_obj<node>()};
        // counted_node_ptr vir_node{1, new node{}};
        ++ construct_cnt;
        auto old_tail = tail.load(std::memory_order_relaxed);

        while (true) {
            // tail.acquire
            increase_external_count(tail, old_tail);
            T *old_data = nullptr;
            if (old_tail.ptr->data.compare_exchange_strong(old_data, new_data, 
                        std::memory_order_acq_rel, std::memory_order_relaxed)) [[unlikely]]{
                counted_node_ptr vir_assist = { 0 };
                if (!old_tail.ptr->next.compare_exchange_strong(vir_assist, vir_node, 
                                    std::memory_order_acq_rel, std::memory_order_relaxed)) {
                    alloc.template delete_obj<>(vir_node.ptr);
                    // delete vir_node.ptr;
                    vir_node = vir_assist;
                    ++ destruct_cnt;
                }
                // tail.release
                set_new_tail(old_tail, vir_node);
                break;
            }
            else {
                counted_node_ptr vir_assist = { 0 };
                if (old_tail.ptr->next.compare_exchange_strong(vir_assist, vir_node, 
                                std::memory_order_acq_rel, std::memory_order_relaxed)) [[likely]]{
                    vir_assist = vir_node;
                    // vir_node管理的node已经交给vir_assist，为了新一轮自己的push，所以自己再开辟一份node
                    vir_node.ptr = alloc.template new_obj<node>();
                    // vir_node.ptr = new node{};
                    ++ construct_cnt;
                }
                // tail.release
                set_new_tail(old_tail, vir_assist);
            }
        }
    }


    template <class T, bool Mod, class Allocater>
    [[nodiscard]] std::optional<T> lock_free_queue<T, Mod, Allocater>::pop() {
        auto old_head = head.load(std::memory_order_relaxed);
        while (true) {
            // head.acquire
            increase_external_count(head, old_head);
            auto *const ptr = old_head.ptr;
            if (ptr == tail.load(std::memory_order_acquire).ptr) {
                ptr->release_ref();
                return std::nullopt;
            }
            
            auto next = ptr->next.load(std::memory_order_relaxed);
            // head.release
            if (head.compare_exchange_strong(old_head, next, std::memory_order_release, std::memory_order_relaxed)) {
                T *const res = ptr->data.exchange(nullptr, std::memory_order_relaxed);
                auto opt = *res;
                alloc.template delete_obj<>(res);
                // delete res;
                free_external_counter(old_head);
                ++ task_cnt;
                return { opt };
            }
            ptr->release_ref();
        }
    }


    template <class T, bool Mod, class Allocater>
    void lock_free_queue<T, Mod, Allocater>::increase_external_count(std::atomic<counted_node_ptr> &counter, counted_node_ptr &old_counter) {
        counted_node_ptr new_counter;
        do {
            new_counter = old_counter;
            ++ new_counter.external_count;
        }
        while (!counter.compare_exchange_strong(old_counter, new_counter, std::memory_order_acquire, std::memory_order_relaxed));
        old_counter.external_count = new_counter.external_count;
    }


    // 执行成功线程调用此函数：减少push&pop计数器，获取push&push or pop&pop计数器
    // 判断当前 操作相同线程有多少
    template <class T, bool Mod, class Allocater>
    void lock_free_queue<T, Mod, Allocater>::free_external_counter(counted_node_ptr &old_node_ptr) {
        node *const ptr = old_node_ptr.ptr;
        // 当前除自己外，还有多少push线程指涉这个节点
        const int increase_count = old_node_ptr.external_count - 2;

        auto old_counter = ptr->count.load(std::memory_order_relaxed);
        node_counter new_counter;
        do {
            new_counter = old_counter;
            -- new_counter.external_counters;
            new_counter.internal_count += increase_count;
        }
        while (!ptr->count.compare_exchange_strong(old_counter, new_counter, std::memory_order_acq_rel, std::memory_order_relaxed));
        if (!new_counter.external_counters && !new_counter.internal_count) { alloc.template delete_obj<>(ptr); ++ destruct_cnt; }
        // if (!new_counter.external_counters && !new_counter.internal_count) { delete ptr; ++ destruct_cnt; }
    }


    // 执行失败线程调用此函数：减少 相同操作线程个数
    template <class T, bool Mod, class Allocater>
    void lock_free_queue<T, Mod, Allocater>::node::release_ref() {
    auto old_counter = count.load(std::memory_order_relaxed);
        node_counter new_counter;
        do {
            new_counter = old_counter;
            -- new_counter.internal_count;
        }
        while (!count.compare_exchange_strong(old_counter, new_counter, std::memory_order_acq_rel, std::memory_order_relaxed));
        if (!new_counter.external_counters && !new_counter.internal_count) { alloc.template delete_obj<>(this); ++ destruct_cnt; }
        // if (!new_counter.external_counters && !new_counter.internal_count) { delete this; ++ destruct_cnt; }
    }


    template <class T, bool Mod, class Allocater>
    void lock_free_queue<T, Mod, Allocater>::set_new_tail(counted_node_ptr &old_tail, counted_node_ptr const& new_tail) {
        // 比较指针效率高
        node *const current_tail_ptr = old_tail.ptr;
        // 第一个线程tail.compare_exchange_weak(old_tail, new_tail), 成功后
        // 第二个线程判断old_tail.ptr == current_tail_ptr, 直接退出while
        while (!tail.compare_exchange_weak(old_tail, new_tail, 
                                           std::memory_order_release, std::memory_order_relaxed) && old_tail.ptr == current_tail_ptr);
        if (old_tail.ptr == current_tail_ptr)
            free_external_counter(old_tail);
        else
            current_tail_ptr->release_ref();
    }


    template <class T, bool Mod, class Allocater>
    lock_free_queue<T, Mod, Allocater>::~lock_free_queue() {
        while (pop().has_value()) {}
        auto cur_head = head.load(std::memory_order_relaxed);
        alloc.template delete_obj<>(cur_head.ptr);
        // delete cur_head.ptr;
        ++ destruct_cnt;
        // 开O3优化，乱序执行，所以task个数会不够
        if constexpr (!Mod) minilog::log_info("construct_cnt: {}, destruct_cnt: {}, task_cnt: {}", construct_cnt.load(), destruct_cnt.load(), task_cnt.load());
        // assert(construct_cnt == destruct_cnt); 
    }
}
#endif

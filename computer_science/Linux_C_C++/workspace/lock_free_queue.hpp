#ifndef LOCK_FREE_QUEUE
#define LOCK_FREE_QUEUE


#include <iostream>
#include <type_traits>
#include <optional>
#include <memory>
#include <atomic>
#include <unistd.h>



namespace lockfree {
    std::atomic<int> new_cnt;
    std::atomic<int> data_cnt;
    std::atomic<int> node_cnt;
    std::atomic<int> dlt_cnt;
    std::atomic<int> pop_cnt;
    // true 为release; false 为debug
    template <class T, bool flag>
    class lock_free_queue {
    private:
        struct node {
            std::atomic<T*> data;
            std::atomic<node*> next;
            std::atomic<int> node_counts;        
            
            node(): data(nullptr), next(nullptr), node_counts(2) {}
            ~node() { node_counts.store(0); }
        };
        int counts = 3;
        std::atomic<node*> head, tail;
    public:
        lock_free_queue();
        ~lock_free_queue();
        void push(T data);
        std::optional<T> pop();
    };



    template <class T, bool flag>
    lock_free_queue<T, flag>::lock_free_queue() {
        if constexpr (!flag) std::cout << "construct\n";
        node *ptr = new node{};
        ++ new_cnt;
        head.store(ptr, std::memory_order_relaxed);
        tail.store(ptr, std::memory_order_relaxed);
    }


    template <class T, bool flag>
    void lock_free_queue<T, flag>::push(T data) {
        if constexpr (!flag) std::cout << "push\n";

        T *data_ptr = new T(data);
        ++ new_cnt;
        node *vir_own = new node{};
        ++ new_cnt;

        while (true) {
            node *old_tail = tail.load(std::memory_order_relaxed);
            ++ old_tail->node_counts;
            T *old_data = nullptr;
            node *old_next = nullptr;
            
            // 先对vir节点中存入的data进行同步
            if (old_tail->data.compare_exchange_strong(old_data, data_ptr, std::memory_order_acq_rel, std::memory_order_relaxed)) [[likely]] {
                // 再对更新tail指针进行同步，同时接受另一个线程的协助
                if (!old_tail->next.compare_exchange_strong(old_next, vir_own, std::memory_order_acq_rel, std::memory_order_relaxed)) [[unlikely]] {
                    delete vir_own;
                    ++ dlt_cnt;
                    ++ node_cnt;
                }
                else tail.store(vir_own, std::memory_order_release);
                -- old_tail->node_counts;
                break;
            }
            else {
                // 协助其他线程
                node *vir_assist = new node{};
                ++ new_cnt;
                if (old_tail->next.compare_exchange_strong(old_next, vir_assist, std::memory_order_acq_rel, std::memory_order_relaxed)) [[likely]] {
                    tail.store(vir_assist, std::memory_order_release);
                }
                else {
                    delete vir_assist;
                    ++ dlt_cnt;
                    ++ node_cnt;
                }
                -- old_tail->node_counts;
            }
        }
    }

    template <class T, bool flag>
    std::optional<T> lock_free_queue<T, flag>::pop() {
        if constexpr (!flag) std::cout << "pop\n";
    
        while (true) {
            node *old_head = head.load(std::memory_order_relaxed);
            auto temp_head = old_head;
            ++ old_head->node_counts;

            // 与push中的tail同步
            if (old_head == tail.load(std::memory_order_acquire)) [[unlikely]] {
                -- old_head->node_counts;
                return { std::nullopt };
            }

            // 弹出数据
            node *next = old_head->next.load(std::memory_order_relaxed);
            if (!head.compare_exchange_strong(old_head, next, std::memory_order_acq_rel, std::memory_order_relaxed)) [[likely]] {
                // core dumped
                -- temp_head->node_counts;
            }
            else {
                ++ pop_cnt;
                T *data = old_head->data.exchange(nullptr, std::memory_order_acq_rel);
                auto opt = *data;
                delete data;
                ++ dlt_cnt;
                ++ data_cnt;
                if (counts == old_head->node_counts.load()) {
                    // core dumped
                    delete old_head;
                    ++ dlt_cnt;
                    ++ node_cnt;
                }
                else -- old_head->node_counts;
                return { opt };
            }
        }
    }

    template <class T, bool flag>
    lock_free_queue<T, flag>::~lock_free_queue() {
        if constexpr (!flag) std::cout << "deconstruct\n";
        std::optional<T> opt = pop();
        while (opt.has_value()) {
            opt = pop();
        }
        delete head.load();
        ++ dlt_cnt;
        ++ node_cnt;
        std::cout << new_cnt << " " << dlt_cnt << " " << data_cnt << " " << node_cnt << " " << pop_cnt << "\n";
    }
}


#endif // LOCK_FREE_QUEUE

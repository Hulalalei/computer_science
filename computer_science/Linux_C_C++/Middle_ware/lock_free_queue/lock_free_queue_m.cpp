#ifndef LOCK_FREE_QUEUE
#define LOCK_FREE_QUEUE


#include <iostream>
#include <type_traits>
#include <optional>
#include <memory>
#include <atomic>
#include <unistd.h>



namespace lockfree {
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

        void try_dlt(node *);
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
        head.store(ptr, std::memory_order_relaxed);
        tail.store(ptr, std::memory_order_relaxed);
    }


    template <class T, bool flag>
    void lock_free_queue<T, flag>::push(T data) {
        if constexpr (!flag) std::cout << "push\n";
        // bug04 push不进行引用计数，否则会影响pop删除节点
        T *data_ptr = new T(data);
        node *vir_own = new node{};

        while (true) {
            node *old_tail = tail.load(std::memory_order_relaxed);
            T *old_data = nullptr;
            node *old_next = nullptr;
            
            // bug05 有可能push线程还没退出，pop就已经删除节点了，然后core dumped
            // 先对vir节点中存入的data进行同步
            if (old_tail->data.compare_exchange_strong(old_data, data_ptr, std::memory_order_acq_rel, std::memory_order_relaxed)) [[likely]] {
                // 再对更新tail指针进行同步，同时接受另一个线程的协助
                if (!old_tail->next.compare_exchange_strong(old_next, vir_own, std::memory_order_acq_rel, std::memory_order_relaxed)) [[unlikely]] {
                    delete vir_own;
                }
                else tail.store(vir_own, std::memory_order_release);
                break;
            }
            else {
                // 协助其他线程
                node *vir_assist = new node{};
                if (old_tail->next.compare_exchange_strong(old_next, vir_assist, std::memory_order_acq_rel, std::memory_order_relaxed)) [[likely]] {
                    tail.store(vir_assist, std::memory_order_release);
                }
                else {
                    delete vir_assist;
                }
            }
        }
    }

    template <class T, bool flag>
    std::optional<T> lock_free_queue<T, flag>::pop() {
        if constexpr (!flag) std::cout << "pop\n";
    
        while (true) {
            // bug01: 必须保证，当前节点删除后，其余线程读取不到之前的值        
            node *old_head = head.load(std::memory_order_seq_cst);
            node *temp_head = old_head;
            ++ old_head->node_counts;
            // std::cout << old_head->node_counts.load() << std::endl;
            // 与push中的tail同步
            if (old_head == tail.load(std::memory_order_acquire)) [[unlikely]] {
                -- old_head->node_counts;
                return { std::nullopt };
            }

            // 弹出数据
            // bug02: 当前else线程可能还没退出，其他线程if就已经退出获取下一个head了
            // 尽可能的让else多睡会，使得其他线程全部退出
            node *next = old_head->next.load(std::memory_order_relaxed);
            if (!head.compare_exchange_strong(temp_head, next, std::memory_order_acq_rel, std::memory_order_relaxed)) [[likely]] {
                try_dlt(old_head);
            }
            else {
                T *data = old_head->data.exchange(nullptr, std::memory_order_acq_rel);
                auto opt = *data;
                delete data;
                try_dlt(old_head);
                return { opt };
            }
        }
    }

    template <class T, bool flag>
    void lock_free_queue<T, flag>::try_dlt(node *old_head) {
        // bug03 thread 1在--之前，thread 2进行判断；
        // thread 2判断失败后，退出，此时thread 1 --counts完成，也退出
        if (counts == old_head->node_counts --) {
            delete old_head;
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
    }
}

#endif // LOCK_FREE_QUEUE

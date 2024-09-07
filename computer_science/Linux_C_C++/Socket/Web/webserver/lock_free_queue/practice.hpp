#include <iostream>
#include <atomic>
#include <thread>
#include <memory>


template <typename T>
class lock_free_queue {
private:
    struct node {
        node() = default;
        std::atomic<T*> data;
        std::atomic<node*> next;
    };
    std::atomic<node*> head;
    std::atomic<node*> tail;
public:
    void push(T data);
    std::unique_ptr<T> pop();
};

template <typename T>
lock_free_queue<T>::lock_free_queue() {
    auto virtual_node = std::make_unique<node>();
    head.store(new_node.get(), std::memory_order_relaxed);
    tail.store(new_node.get(), std::memory_order_relaxed);
}

template <typename T>
lock_free_queue<T>::node::node() {
    next.store(nullptr, std::memory_order_relaxed);
}

// 队尾进
template <typename T>
void lock_free_queue<T>::push(T data) {
    auto new_data = std::make_unique<T>(data);
    node *new_next = new node;
    auto old_tail = tail.load(std::memory_order_relaxed);

    while (true) {
        T *old_data = nullptr;
        if (old_tail->data.compare_exchange_strong(old_data, new_data.get())) {
            node *old_next = nullptr;
            if (!old_tail->next.compare_exchange_strong(old_next, new_next)) {
                delete new_next;
                new_next = old_next;
            }
            set_new_tail(old_tail, new_next);
            break;
        }
        else {
            node *old_next = nullptr;
            if(old_tail->next.compare_exchange_strong(old_next, new_next)) {
                set_new_tail(old_tail, new_next);
            }
        }
    }
}


template <typename T>
std::unique_ptr<T> lock_free_queue<T>::pop() {
    auto old_head = head.load(std::memory_order_relaxed);
    while (true) {
        if (old_head == tail.load(std::memory_order_relaxed)) return make_unique<T>();
        if (head.compare_exchange_strong(old_head, old_head->next.load(std::memory_order_relaxed))) {
            auto res = old_head->data.exchange(nullptr, std::memory_order_relaxed);
            return std::unique_ptr<T>(res));
        }
    }
}



int main(void) {




    return 0;
}
#include <iostream>
#include <thread>
#include <boost/atomic/atomic.hpp>
#include <memory>



template <typename T>
class lock_free_stack {
public:
    lock_free_stack() {}
    void push(T const& data);
    std::shared_ptr<T> pop();
    lock_free_stack(lock_free_stack const& obj) = delete;
    lock_free_stack& operator=(lock_free_stack const& obj) = delete;
    ~lock_free_stack();
private:
    struct node;
    struct counted_node_ptr {
        int external_count;
        node *ptr;
    };
    struct node {
        std::shared_ptr<T> data;
        boost::atomic<int> internal_count;
        counted_node_ptr next;
        node(T const& data):
            data(std::make_shared<T>(std::move(data))), internal_count(0) {}
    };
    boost::atomic<counted_node_ptr> head;
    void increase_head_count(counted_node_ptr& old_counter);
};

template <typename T>
void lock_free_stack<T>::push(T const& data) {
    counted_node_ptr new_node;
    new_node.ptr = new node(data);
    new_node.external_count = 1;
    new_node.ptr->next = head.load(boost::memory_order_relaxed);
    // 同步2
    while (!head.compare_exchange_weak(new_node.ptr->next, new_node, 
	boost::memory_order_release, boost::memory_order_relaxed));
}

template <typename T>
std::shared_ptr<T> lock_free_stack<T>::pop() {
    counted_node_ptr old_head = head.load(boost::memory_order_relaxed);
    for (;;) {
        // 同步2
        increase_head_count(old_head);
        node* const ptr = old_head.ptr;
        if (!ptr) {
            return std::shared_ptr<T>();
        }
        if (head.compare_exchange_strong(old_head, ptr->next, boost::memory_order_relaxed)) {
            std::shared_ptr<T> res;
            res.swap(ptr->data);
            int const count_increase = old_head.external_count - 2;
            // 同步1
            if (ptr->internal_count.fetch_add(count_increase, boost::memory_order_release) == -count_increase) 
                delete ptr;
            return res;
        }
        else if (ptr->internal_count.fetch_add(-1, boost::memory_order_relaxed) == 1) {
            // 同步1
            ptr->internal_count.load(boost::memory_order_acquire);
            delete ptr;
	    }
    }
}

template <typename T>
void lock_free_stack<T>::increase_head_count(counted_node_ptr& old_counter) {
    counted_node_ptr new_counter;
    do {
        new_counter = old_counter;
        ++new_counter.external_count;
    }
    while (!head.compare_exchange_strong(old_counter, new_counter, 
	boost::memory_order_acquire, boost::memory_order_relaxed));
    old_counter.external_count = new_counter.external_count;
}

template <typename T>
lock_free_stack<T>::~lock_free_stack() {
    while (pop());
}

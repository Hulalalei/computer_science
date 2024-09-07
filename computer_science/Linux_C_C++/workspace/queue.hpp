#include <optional>
#include <iostream>
#include <memory>
#include <mutex>


template <class T, bool flag>
class queue {
private:
    std::mutex comm_mutex;
    std::mutex head_mutex;
    std::mutex tail_mutex;
    struct node {
        std::shared_ptr<T> data;
        std::shared_ptr<node> next;
    };
public:
    queue();
    void push(T data);
    std::optional<T> pop();
    std::shared_ptr<node> head;
    node *tail;
};
 
// head -> 1 -> 2 -> vir(tail)
template <class T, bool flag>
queue<T, flag>::queue() {
    if constexpr (!flag) std::cout << "construct\n";
    head = std::make_shared<node>();
    tail = head.get();
}

template <class T, bool flag>
void queue<T, flag>::push(T data) {
    if constexpr (!flag) std::cout << "push\n";

    auto vir = std::make_shared<node>();
    auto new_vir = vir.get();
    auto ptr = std::make_shared<T>(data);

    std::lock_guard<std::mutex> guard(tail_mutex);    
    tail->data = std::move(ptr);
    tail->next = std::move(vir);
    tail = new_vir;
}

template <class T, bool flag>
std::optional<T> queue<T, flag>::pop() {
    if constexpr (!flag) std::cout << "pop\n";

    std::unique_lock<std::mutex> head_lock(head_mutex);
    std::unique_lock<std::mutex> tail_lock(tail_mutex);
    if (head.get() == tail) return {};

    auto t = head;
    head = std::move(head->next);
    head_lock.unlock();

    auto opt = std::move(t->data);
    return { *opt };
}

#ifndef LOCK_FREE_QUEUE_HPP
#define LOCK_FREE_QUEUE_HPP


#include <iostream>
#include <memory>
#include <boost/atomic/atomic.hpp>


namespace web {
// head -> node -> tail
// 1、内存序同步，一般指同一个原子变量在多线程中的同步，单线程下内存序无意义
// 同步也仅仅是在多线程中的同步
// ！！！不用硬件处理方式不同！！！
// 数据结构 越复杂，还是倾向于使用锁比较好
template <typename T>
class lock_free_queue {
private:
	struct node;
	struct counted_node_ptr {
		int external_count;
		node *ptr;
		counted_node_ptr():external_count(0), ptr(nullptr) {}
	};
	boost::atomic<counted_node_ptr> head;
	boost::atomic<counted_node_ptr> tail;
	struct node_counter {
		unsigned int internal_count: 30;
		unsigned int external_counters: 2;
	};
	struct node {
		boost::atomic<T*> data;
		boost::atomic<node_counter> count;
		boost::atomic<counted_node_ptr> next;
		node();
		void release_ref();
	};
private:
	void set_new_tail(counted_node_ptr &old_tail, counted_node_ptr const& new_tail);
    static void increase_external_count(boost::atomic<counted_node_ptr> &counter, counted_node_ptr &old_counter);
    static void free_external_counter(counted_node_ptr &old_node_ptr);
public:
    lock_free_queue();
    void push(T new_value);
    std::unique_ptr<T> pop();
    ~lock_free_queue();
};

// 2、队头队尾有虚位节点
// 关于头节点和尾结点进行内存序放松，store在load之前
template <typename T>
lock_free_queue<T>::lock_free_queue() {
	counted_node_ptr new_next;
	new_next.ptr = new node();
	new_next.external_count = 1;
	tail.store(new_next, boost::memory_order_relaxed);
	head.store(new_next, boost::memory_order_relaxed);
}

// 插入队尾后，head仍然指向第一个数据，tail指虚位节点
// CAS后，即使智能指针销毁，也能继续访问此内存
template <typename T>
void lock_free_queue<T>::push(T new_value) {
	std::unique_ptr<T> new_data(new T(new_value));
	counted_node_ptr new_next;
	new_next.ptr = new node;
	new_next.external_count = 1;
	counted_node_ptr old_tail = tail.load(boost::memory_order_relaxed);
	
	for (;;) {
		increase_external_count(tail, old_tail);
		T *old_data = nullptr;

		// tail存储数据data有条件竞争，如果relaxed，则存储两次...err
		if (old_tail.ptr->data.compare_exchange_strong(old_data, new_data.get())) {
			counted_node_ptr old_next;
            counted_node_ptr now_next = old_tail.ptr->next.load();
            // 队尾非空，与pop中head有条件竞争
            // 也可以和push竞争，例如提前更新了tail：
			if (!old_tail.ptr->next.compare_exchange_strong(old_next, new_next,
														boost::memory_order_release, boost::memory_order_relaxed)) {
				delete new_next.ptr;
				// ptr->next给old_next， old_next又给了new_next
				new_next = old_next;
			}
			set_new_tail(old_tail, new_next);
			new_data.release();
			break;
		}
		else {
		// 如果当前有其他线程已经存储了数据data，则else内可以协助其进行链接尾部
			counted_node_ptr old_next;
			if (old_tail.ptr->next.compare_exchange_strong(old_next, new_next,
														boost::memory_order_release, boost::memory_order_relaxed)) {
				// old_next = new_next;
				// old_tail->next指向的是new_next，所以new_next创建堆区内存；
				// 而且仅仅更改了old_tail的指向，还未更改tail的值
				// 而old_next,err
				new_next.ptr = new node;
			}
			set_new_tail(old_tail, new_next);
		}
	}
}

// 弹出队头
template <typename T>
std::unique_ptr<T> lock_free_queue<T>::pop() {
	counted_node_ptr old_head = head.load(boost::memory_order_relaxed);
	for (;;) {
	// 1和2均执行，可以保证引用计数的正确性
	// 1
		increase_external_count(head, old_head);
		node *const ptr = old_head.ptr;
		// 同步：在set_new_tail后
		if (ptr == tail.load(boost::memory_order_acquire).ptr) {
			ptr->release_ref();
			return std::unique_ptr<T>();
		}
		// 同步：获取next之前，tail应该已经改变完
		counted_node_ptr next = ptr->next.load(boost::memory_order_acquire);
		if (head.compare_exchange_strong(old_head, next,
									boost::memory_order_release, boost::memory_order_relaxed)) {
			// 仅有当前线程能访问data
			T *const res = ptr->data.exchange(nullptr, boost::memory_order_relaxed);
			free_external_counter(old_head);
			return std::unique_ptr<T>(res);
		}
	// 2
		ptr->release_ref();
	}
}

template <typename T>
lock_free_queue<T>::node::node() {
	node_counter new_count;
	new_count.internal_count = 0;
	new_count.external_counters = 2;
	count.store(new_count, boost::memory_order_relaxed);
	
	counted_node_ptr node_ptr;
	node_ptr.ptr = nullptr;
	node_ptr.external_count = 0;
	next.store(node_ptr, boost::memory_order_relaxed);
}

// node_counter.memory_order_acquire
template <typename T>
void lock_free_queue<T>::node::release_ref() {
	node_counter old_counter = count.load(boost::memory_order_relaxed);
	node_counter new_counter;
	do {
		new_counter = old_counter;
		-- new_counter.internal_count;
	}
	while (!count.compare_exchange_strong(old_counter, new_counter, 
		boost::memory_order_acquire, boost::memory_order_relaxed));
	if (!new_counter.internal_count && !new_counter.external_counters)
		delete this;
}

// tail.memory_order_acquire
template <typename T>
void lock_free_queue<T>::increase_external_count(
	boost::atomic<counted_node_ptr> &counter, counted_node_ptr &old_counter) {
	counted_node_ptr new_counter;
	do {
		new_counter = old_counter;
		++ new_counter.external_count;
	}
	while (!counter.compare_exchange_strong(old_counter, new_counter, 
		boost::memory_order_acquire, boost::memory_order_relaxed));
	old_counter.external_count = new_counter.external_count;
}

// node_counter.memory_order_acquire
template <typename T>
void lock_free_queue<T>::free_external_counter(counted_node_ptr &old_node_ptr) {
	node *const ptr = old_node_ptr.ptr;
	int const count_increase = old_node_ptr.external_count - 2;
	node_counter old_counter = ptr->count.load(boost::memory_order_relaxed);
	node_counter new_counter;
	do {
		new_counter = old_counter;
		-- new_counter.external_counters;
		new_counter.internal_count += count_increase;
	}
	while (!ptr->count.compare_exchange_strong(old_counter, new_counter, 
		boost::memory_order_acquire, boost::memory_order_relaxed));
	if (!new_counter.internal_count && !new_counter.external_counters)
		delete ptr;
}

// tail.set_new_tail
// free_external_counter
// release_ref
template <typename T>
void lock_free_queue<T>::set_new_tail(counted_node_ptr &old_tail, counted_node_ptr const& new_tail) {
	node *const current_tail_ptr = old_tail.ptr;
	while (!tail.compare_exchange_weak(old_tail, new_tail) && old_tail.ptr == current_tail_ptr);
	if (old_tail.ptr == current_tail_ptr)
		free_external_counter(old_tail);
	else
		current_tail_ptr->release_ref();
}

template <typename T>
lock_free_queue<T>::~lock_free_queue() {
	while (pop());
	auto head_counted_node = head.load(boost::memory_order_relaxed);
	delete head_counted_node.ptr;
}

}

#endif
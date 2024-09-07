#include <iostream>
#include <memory>
#include <boost/atomic/atomic.hpp>



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


template <typename T>
lock_free_queue<T>::lock_free_queue() {
	counted_node_ptr new_next;
	new_next.ptr = new node();
	new_next.external_count = 1;
	tail.store(new_next);
	head.store(new_next);
}


template <typename T>
void lock_free_queue<T>::push(T new_value) {
	std::unique_ptr<T> new_data(new T(new_value));
	counted_node_ptr new_next;
	new_next.ptr = new node;
	new_next.external_count = 1;
	counted_node_ptr old_tail = tail.load();

	for (;;) {
		increase_external_count(tail, old_tail);
		T *old_data = nullptr;


		if (old_tail.ptr->data.compare_exchange_strong(old_data, new_data.get())) {
			counted_node_ptr old_next;
            counted_node_ptr now_next = old_tail.ptr->next.load();

			if (!old_tail.ptr->next.compare_exchange_strong(old_next, new_next)) {
				delete new_next.ptr;

				new_next = old_next;
			}
			set_new_tail(old_tail, new_next);
			new_data.release();
			break;
		}
		else {

			counted_node_ptr old_next;
			if (old_tail.ptr->next.compare_exchange_strong(old_next, new_next)) {
				old_next = new_next;
				new_next.ptr = new node;
			}
			set_new_tail(old_tail, old_next);
		}
	}
}


template <typename T>
std::unique_ptr<T> lock_free_queue<T>::pop() {
	counted_node_ptr old_head = head.load();
	for (;;) {

		increase_external_count(head, old_head);
		node *const ptr = old_head.ptr;

		if (ptr == tail.load().ptr) {
			ptr->release_ref();
			return std::unique_ptr<T>();
		}

		counted_node_ptr next = ptr->next.load();
		if (head.compare_exchange_strong(old_head, next)) {

			T *const res = ptr->data.exchange(nullptr);
			free_external_counter(old_head);
			return std::unique_ptr<T>(res);
		}

		ptr->release_ref();
	}
}

template <typename T>
lock_free_queue<T>::node::node() {
	node_counter new_count;
	new_count.internal_count = 0;
	new_count.external_counters = 2;
	count.store(new_count);

	counted_node_ptr node_ptr;
	node_ptr.ptr = nullptr;
	node_ptr.external_count = 0;
	next.store(node_ptr);
}


template <typename T>
void lock_free_queue<T>::node::release_ref() {
	node_counter old_counter = count.load();
	node_counter new_counter;
	do {
		new_counter = old_counter;
		-- new_counter.internal_count;
	}
	while (!count.compare_exchange_strong(old_counter, new_counter));
	if (!new_counter.internal_count && !new_counter.external_counters)
		delete this;
}


template <typename T>
void lock_free_queue<T>::increase_external_count(
	boost::atomic<counted_node_ptr> &counter, counted_node_ptr &old_counter) {
	counted_node_ptr new_counter;
	do {
		new_counter = old_counter;
		++ new_counter.external_count;
	}
	while (!counter.compare_exchange_strong(old_counter, new_counter));
	old_counter.external_count = new_counter.external_count;
}


template <typename T>
void lock_free_queue<T>::free_external_counter(counted_node_ptr &old_node_ptr) {
	node *const ptr = old_node_ptr.ptr;
	int const count_increase = old_node_ptr.external_count - 2;
	node_counter old_counter = ptr->count.load();
	node_counter new_counter;
	do {
		new_counter = old_counter;
		-- new_counter.external_counters;
		new_counter.internal_count += count_increase;
	}
	while (!ptr->count.compare_exchange_strong(old_counter, new_counter));
	if (!new_counter.internal_count && !new_counter.external_counters)
		delete ptr;
}


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
	auto head_counted_node = head.load();
	delete head_counted_node.ptr;
}

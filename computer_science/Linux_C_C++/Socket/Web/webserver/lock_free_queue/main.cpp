#include <iostream>
//#include "lock.hpp"
#include "lock_free_queue.hpp"



int main(int argc, char **argv) {
	lock_free_queue<int> q; 
	q.push(10);
	q.pop();
	q.push(20);
	return 0;
}

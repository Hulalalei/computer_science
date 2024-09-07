debugf : 
	ldconfig
	g++ -g main.cpp ../lock_free_queue/lock_free_queue.hpp thread_pool.hpp -std=c++17 -l boost_system -l boost_atomic -l pthread -o main

.PHONY : debugf

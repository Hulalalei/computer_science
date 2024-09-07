#include "ThreadPool.h"

void targetTask (void *arg) {
	int num = *(static_cast<int*> (arg));
	std::cout << "num = " << num << std::endl;
}



int main (void) {
	std::cout << "最小线程数，最大线程数，队列容量" << std::endl;
	int Tmin = 0, Tmax = 0, capacity = 0;
	std::cin >> Tmin >> Tmax >> capacity;
	ThreadPool* threadPool = new ThreadPool (Tmin, Tmax, capacity); 
	
	
	
	for (int i = 0; i < 101; i++) {
		int *num = new int;
		*num = i + 100;
		threadPool->addTasks (TaskQueue (targetTask, num));
	}
	
	sleep(15);

	for (int j = 101; j < 201; j++) {
		int *num = new int;
		*num = j + 100;
		threadPool->addTasks (TaskQueue (targetTask, num));
	}	

	sleep(20);


	delete threadPool;
	return 0;
}

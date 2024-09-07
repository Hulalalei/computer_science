#include <iostream>
#include <pthread.h>


//global para rwlock
pthread_rwlock_t rwlock;

//global num
int number = 0;

//readTasks
void* readTasks (void* arg) {
	for (int i = 0; i < 50; i++) {
		pthread_rwlock_rdlock (&rwlock);
		int cur = number;	
		std::cout << "current thread Id = " << pthread_self() << "cur = " << cur << std::endl;
		pthread_rwlock_unlock (&rwlock);
	}
}

//writeTasks
void* writeTasks (void *arg) {
	for (int j = 0; j < 50; j++) {
		pthread_rwlock_wrlock (&rwlock);
		number++;
		int cur = number;
		std::cout << "current thread Id = " << pthread_self() << "cur = " << cur << std::endl;
		pthread_rwlock_unlock (&rwlock);
	}
}



int main (void) {
	//init the rwlock
	pthread_rwlock_init (&rwlock, NULL);	

	//create threads
	pthread_t readId[5], writeId[3];
	for (int i = 0; i < 5; i++) {
		pthread_create (&readId[i], NULL, readTasks, NULL);
	}
	for (int j = 0; j < 3; j++) {
		pthread_create (&writeId[j], NULL, writeTasks, NULL);
	}

	//recycle son threads' resource
	for (int i = 0; i < 5; i++) {
		pthread_join (readId[i], NULL);
	}
	for (int j = 0; j < 3; j++) {
		pthread_join (writeId[j], NULL);
	}

	//destory the locks
	pthread_rwlock_destroy (&rwlock);
}

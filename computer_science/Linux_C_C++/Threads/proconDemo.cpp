#include <iostream>
#include <pthread.h>
#include <vector>
#include <unistd.h>
#include <semaphore.h>


class Node {
public:
	int number;	
};

static std::vector<Node> blockQueue;
pthread_mutex_t mutex;
pthread_cond_t cond;
sem_t semp;
sem_t semc;

void* ProducerTasks (void *arg) {
	while (true) {
		//semaphore		
		sem_wait(&semp);		


		//lock the critical resource
		pthread_mutex_lock (&mutex);
		
		//producting
		Node* node = new Node();
		node->number = rand() % 100;		
		std::cout << "the producer thread Id = " << pthread_self() << " number = " << node->number << std::endl;	
		//std::cout << "queue size = " << blockQueue.size () << std::endl;
		blockQueue.push_back (*node);
			
		//unlock the critical resource
		pthread_mutex_unlock (&mutex);
		
		//awake the producer threads
		//pthread_cond_signal (&cond);
		sem_post(&semc);
		sleep(3);
	}
}


void* ConsumerTasks (void *arg) {
	while (true) {
		//semaphore
		sem_wait(&semc);


		//lock the critical resource
		pthread_mutex_lock (&mutex);
		
		//if queue is empty
		//put the while in the critical resource.
		//while (blockQueue.empty()) {
			//pthread_cond_wait (&cond, &mutex);
		//}
		

		//consumering
		Node* node = &blockQueue.back ();
		blockQueue.pop_back ();
		std::cout << "the consumer thread Id = " << pthread_self () << "number = " << node->number << std::endl;
		
		//unlock the critical resource
		pthread_mutex_unlock (&mutex);
	

		//awake
		sem_post(&semp);	
		sleep (1);
	}
}



int main (void) {
	//init the mutex, cond and semaphore
	pthread_mutex_init (&mutex, NULL);
	pthread_cond_init (&cond, NULL);
	sem_init (&semp, 0, 5);
	sem_init (&semc, 0, 0);

	//create threads
	pthread_t proId[5], conId[5];
	for (int i = 0; i < 5; i++) {
		pthread_create (&proId[i], NULL, ProducerTasks, NULL);
		pthread_create (&conId[i], NULL, ConsumerTasks, NULL);
	}
	
	//for (int i = 0; i < 5; i++) {
		//pthread_create (&conId[i], NULL, ConsumerTasks, NULL);
	//}

	//recycle threads
	for (int j = 0; j < 5; j++) {
		pthread_join (proId[j], NULL);
		pthread_join (conId[j], NULL);
	}


	//destroy the locks, conds and sems
	pthread_mutex_destroy (&mutex);
	pthread_cond_destroy (&cond);
	sem_destroy (&semp);
	sem_destroy (&semc);
}






























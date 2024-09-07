#pragma once
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <vector>
#include <queue>
#include <map>
#include <string>

class TaskQueue {
public:
	//函数指针格式不对，所以报错.
	void (*task) (void *arg);
	void *arg;

	TaskQueue ();
	TaskQueue (void (*func)(void *arg), void *arg);
};


class ThreadPool {
public:
	//variables
	pthread_mutex_t mutexPool;
	pthread_mutex_t mutexWorker;
	pthread_cond_t isFull;
	pthread_cond_t isEmpty;
	

	std::queue<TaskQueue> taskQueue;
	int capacity;
	int busyNum;
	int liveNum;
	int exitNum;

	pthread_t managerId;
	//pthread_t *workerIds;
	std::map<pthread_t, bool> workerIds;
	int minNum;
	int maxNum;
	bool isShutDown;

	//constructor
	ThreadPool ();
	ThreadPool (int minNum, int maxNum, int capacity);

	//add task
	void addTasks(TaskQueue task);

	//get threads num
	int getThreadsNum ();

	//get threads index
	pthread_t getThreadsIndex ();

	//exit
	void threadsExit ();

	//判断是否存在
	bool isExist (pthread_t p);

	//静态，就有地址，如果不是，必须实例化后才有地址
	static void* worker (void *arg);

	//run
	static void* manager (void *arg);

	//destructor
	~ThreadPool ();
};

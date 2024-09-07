#include "ThreadPool.h"
const int threadsNum = 2;

void* ThreadPool::worker (void *arg) {
	ThreadPool *p = static_cast<ThreadPool*> (arg);
	int tnum = 0;

	while (!p->isShutDown) {
		sleep(1);
		
		pthread_mutex_lock (&p->mutexPool);//critical resource
		if (p->workerIds.find (pthread_self ())->second == false) break;

		//std::cout << "workerId = " << pthread_self() << " worker begin to working..." <<std::endl; 

		//判断退出条件
		if (p->taskQueue.size () == 0) {
			//std::cerr << "worker has been blocked" << std::endl;
			pthread_cond_wait (&p->isEmpty, &p->mutexPool);
		}
		if (p->isShutDown) {
			p->threadsExit ();
			goto flag;
		}

		//杀死工作者线程
		pthread_mutex_lock (&p->mutexWorker);
		tnum = p->getThreadsNum ();
		for (int i = 0; i < p->exitNum; i++) {
			if (tnum > p->minNum) {
				//std::cout << "Id = " << pthread_self() << " has been killed" << std::endl;
				p->threadsExit ();
			}
			goto flag;
		}
		pthread_mutex_unlock (&p->mutexWorker);

		//获取任务
		TaskQueue task;
		int flag = 0;
		if (p->taskQueue.size () != 0) {
			task = p->taskQueue.front ();
			p->taskQueue.pop ();
			flag = 1;
		}
		//std::cerr << "已获取任务\n"; 
		pthread_mutex_unlock (&p->mutexPool);

		//唤醒生产者，并且执行任务
		pthread_cond_signal (&p->isFull);
		
		
		if (flag == 1) {
			//std::cerr << "开始执行任务\n";
			task.task (task.arg);
		}
	}

	//线程自杀流程
	flag: //std::cerr << "func is done" << std::endl;
	p->exitNum--;
	pthread_mutex_unlock (&p->mutexWorker);
	pthread_mutex_lock (&p->mutexWorker);
	tnum = p->getThreadsNum ();
	//std::cout << "Id = " << pthread_self () << " workers = " << tnum << std::endl;
	pthread_mutex_unlock (&p->mutexWorker);
	pthread_mutex_unlock (&p->mutexPool);
}

void* ThreadPool::manager (void *arg) {
	ThreadPool *p = static_cast<ThreadPool*> (arg);
	while (!p->isShutDown) {
		sleep (3);
		if (p->isShutDown) break;
		pthread_mutex_lock (&p->mutexPool);
		//std::cout << "managerId = " << pthread_self() << " manager begin to working..." <<std::endl;

		for (int i = 0; i < threadsNum; i++) {
			//获取工作线程
			pthread_mutex_lock (&p->mutexWorker);
			pthread_t worker = p->getThreadsIndex ();
			pthread_mutex_unlock (&p->mutexWorker);

			//获取工作者忙的数目，以及此工作者是否存在
			pthread_mutex_lock (&p->mutexWorker);
			int tnum = p->getThreadsNum ();
			bool flag = p->isExist (worker);
			//std::cerr << "flag = " << flag << std::endl;
			pthread_mutex_unlock (&p->mutexWorker);
			//std::cout << "workerNum = " << tnum << " maxNum = " << p->maxNum << std::endl;

			//线程数 < maxNum && 任务数 > 线程数
			if (tnum < p->maxNum && p->taskQueue.size () > tnum) {
				//std::cout << "add a worker" << std::endl;
				pthread_create (&worker, nullptr, p->worker, p);
				pthread_detach (worker);
				if (!flag) {
					pthread_mutex_lock (&p->mutexWorker);
					auto nnn = p->workerIds.insert (std::make_pair(worker, true));
					pthread_mutex_unlock (&p->mutexWorker);
					if (nnn.second == false) std::cerr << "insert err\n";
					else {
						//std::cerr << "insert ok\n"; 
						tnum = p->getThreadsNum ();
						std::cerr << "threads' num = " << tnum << "\n";
					}
					//std::cerr << "add1\n";
				}
				else {
					pthread_mutex_lock (&p->mutexWorker);
					p->workerIds.at (worker) = true;
					pthread_mutex_unlock (&p->mutexWorker);
					//std::cerr << "add2\n";
				}
			}
		}

		//delete workers		
		//线程数 > 2 * 任务数 && 线程数 > minNum
		pthread_mutex_lock (&p->mutexWorker);
		int tnum = p->getThreadsNum ();
		pthread_mutex_unlock (&p->mutexWorker);
 
		if (tnum > 2 * p->taskQueue.size() && tnum > p->minNum) {
			p->exitNum = threadsNum;
			std::cerr << "kill threads begin" << std::endl;
			pthread_cond_signal (&p->isEmpty);
		}
		pthread_mutex_unlock (&p->mutexPool);
	}
}

ThreadPool::ThreadPool (int minNum, int maxNum, int capacity) {
	this->minNum = minNum;
	this->maxNum = maxNum;

	pthread_mutex_init (&this->mutexPool, nullptr);
	pthread_mutex_init (&this->mutexWorker, nullptr);
	pthread_cond_init (&this->isFull, nullptr);
	pthread_cond_init (&this->isEmpty, nullptr);

	this->capacity = capacity;
	this->liveNum = minNum;
	this->busyNum = minNum;
	this->exitNum = 0;
	this->isShutDown = false;
	
	for (int j = 0; j < this->minNum; j++) {
		pthread_t tid = 0;
		pthread_create (&tid, nullptr, this->worker, this);
		pthread_detach (tid);		//系统自动回收子线程资源
		this->workerIds.insert (std::make_pair(tid, true));
	}
	
	pthread_create (&this->managerId, nullptr, this->manager, this);
}

void ThreadPool::addTasks (TaskQueue task) {
	pthread_mutex_lock (&this->mutexPool);
	while (this->taskQueue.size() >= this->capacity) {
		pthread_cond_wait (&this->isFull, &this->mutexPool);
	}
	this->taskQueue.push(task);
	pthread_mutex_unlock (&this->mutexPool);

	pthread_cond_signal (&this->isEmpty);
}

pthread_t ThreadPool::getThreadsIndex () {
	auto p = this->workerIds.begin ();
	for (p; p != this->workerIds.end (); p++) {
		if (p->second == false) return p->first;
		else return -1;
	}
}

int ThreadPool::getThreadsNum () {
	int temp = 0;
	auto p = this->workerIds.begin ();
	for (p; p!= this->workerIds.end (); p++) {
		if (p->second == true) {
			temp++;
		}
	}
	return temp;
}

void ThreadPool::threadsExit () {
	pthread_t s = pthread_self();
	for (auto &t : this->workerIds) {
		if (t.first == s) {
			t.second = false;
		}
	}
}

bool ThreadPool::isExist (pthread_t p) {
	for (auto &t : this->workerIds) {
		if (t.first == p) return true;
	}
	return false;
}

ThreadPool::~ThreadPool () {
	this->isShutDown = true;
	pthread_cond_broadcast (&this->isEmpty);

	pthread_join (this->managerId, nullptr);
	std::cout << "managerId = " << managerId << " has been exit..." << std::endl;

	pthread_mutex_destroy (&this->mutexPool);
	pthread_mutex_destroy (&this->mutexWorker);
	pthread_cond_destroy (&this->isEmpty);
	pthread_cond_destroy (&this->isFull);
	std::cout << "mutex and cond has been destroyed..." << std::endl;
	std::cout << "======thread pool has been closed...======" << std::endl;
}

TaskQueue::TaskQueue () {
	this->task = nullptr;
	this->arg = nullptr;
}

TaskQueue::TaskQueue (void (*func)(void *arg), void *arg) {
	this->task = func;
	this->arg = arg;
}

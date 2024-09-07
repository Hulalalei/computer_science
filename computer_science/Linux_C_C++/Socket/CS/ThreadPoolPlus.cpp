#include "ThreadPoolPlus.hpp"


const int killsnum = 2;

Task::Task() {
    this->func = nullptr;
    this->arg = nullptr;
}

Task::Task(void (*func)(void *arg), void *arg) {
    this->func = func;
    this->arg = arg;
}

TaskQueue::TaskQueue(int capacity): queueCap(capacity) {
    this->currNum = queue.size();
}

//类内先加载成员变量，然后才加载成员函数
ThreadPool::ThreadPool(int minNum, int maxNum, int capacity): isShut(false), kill(false), killNum(killsnum),
        maxThreadNum(maxNum), minThreadNum(minNum), taskQueue(new TaskQueue(capacity)) {


    //初始化互斥量
    pthread_mutex_init(&this->mutexPool, nullptr);
    pthread_cond_init(&this->isFull, nullptr);
    pthread_cond_init(&this->isEmpty, nullptr);
    
    //创建管理者线程
    pthread_create(&this->managerId, nullptr, this->manager, this);

    //创建消费者线程
    for (size_t i = 0; i < this->minThreadNum; i++) {
        pthread_t tid = 0;
        pthread_create(&tid, nullptr, this->worker, this);
        pthread_detach(tid);    /* 线程分离，内核回收资源 */
        this->addWorker(tid);
    }
}

auto ThreadPool::manager(void *arg) -> void* {
    auto ptr = static_cast<ThreadPool*> (arg);
    while (!ptr->isShut) {
        /* 添加线程 */
        pthread_mutex_lock(&ptr->mutexPool);
        if (ptr->taskQueue->currNum > 1.5 * ptr->workerMaps.size()) {
            if (ptr->workerMaps.size() < ptr->maxThreadNum) {
                pthread_t tid = 0;
                pthread_create(&tid, nullptr, ptr->worker, ptr);
                ptr->addWorker(tid);
            }
        }
        pthread_mutex_unlock(&ptr->mutexPool);

        /* 销毁线程 */
        pthread_mutex_lock(&ptr->mutexPool);
        if (ptr->workerMaps.size() > ptr->taskQueue->currNum && ptr->workerMaps.size() > ptr->minThreadNum) {
            int kNum = ptr->killNum;
            pthread_mutex_unlock(&ptr->mutexPool);
            while (kNum > 0) {
                std::cerr << "begin to kill...\n";
                pthread_cond_signal(&ptr->isEmpty); /* 唤醒至少一个 */
                ptr->kill = true;
                kNum--;
            }
            ptr->kill = false;
        }
        pthread_mutex_unlock(&ptr->mutexPool);
        sleep(1);
    }
}

auto ThreadPool::worker(void *arg) -> void* {
    auto ptr = static_cast<ThreadPool*> (arg);
    
    while (!ptr->isShut) {
        pthread_mutex_lock(&ptr->mutexPool);
        while (ptr->taskQueue->currNum != 0) {
            Task *p = ptr->takeTask();
            pthread_mutex_unlock(&ptr->mutexPool);
            pthread_cond_broadcast(&ptr->isFull);   /* 唤醒 */
            p->func(p->arg);

            pthread_mutex_lock(&ptr->mutexPool);
        }
        while (ptr->taskQueue->currNum == 0) {
            pthread_cond_wait(&ptr->isEmpty, &ptr->mutexPool);
            if (ptr->kill)
                goto flag;
        }
        pthread_mutex_unlock(&ptr->mutexPool);
        sleep(1);
    }
flag:
    ptr->workerMaps[pthread_self()] = false;
    ptr->workerMaps.erase(pthread_self());
    std::cerr << "threadId = " << pthread_self() << " has been killed\n";
}

auto ThreadPool::addTask(void (*func)(void *arg), void *arg) -> bool {
    pthread_mutex_lock(&this->mutexPool);
    while (this->taskQueue->currNum >= this->taskQueue->queueCap) {
        pthread_cond_wait(&this->isFull, &this->mutexPool);
    }
    this->taskQueue->queue.push(new Task(func, arg));
    this->taskQueue->currNum++;
    pthread_mutex_unlock(&this->mutexPool);
    pthread_cond_broadcast(&this->isEmpty);
    return true;
}

auto ThreadPool::takeTask() -> Task* {
    auto task = this->taskQueue->queue.front();
    this->taskQueue->queue.pop();
    this->taskQueue->currNum--;

    return task;
}

auto ThreadPool::addWorker(pthread_t tid) -> bool {
    auto flag = workerMaps.find(tid);   /* 保存在容器中 */
    if ((flag != workerMaps.end()) && flag->second) {
        throw std::runtime_error("已存在该线程");
    } else if (flag == workerMaps.end()) {
        this->workerMaps.insert(std::pair<pthread_t, bool>(tid, true));
        std::cout << "size = " << workerMaps.size() << "\n";
    } else if ((flag != workerMaps.end()) && !flag->second) {
        this->workerMaps[tid] = true;
    }
}

Task::~Task() {
    
}

TaskQueue::~TaskQueue() {

}

ThreadPool::~ThreadPool() {
    this->isShut = true;
    this->kill = true;

    for (size_t i = 0; i < this->minThreadNum; i++)
        pthread_cond_signal(&this->isEmpty);
    pthread_join(this->managerId, nullptr);
    std::cout << "managerId and workerIds have been destroyed...\n";

    pthread_mutex_destroy(&this->mutexPool);
    pthread_cond_destroy(&this->isEmpty);
    pthread_cond_destroy(&this->isFull);
    std::cout << "mutex and conds have been destroyed...\n";
    
    delete this->taskQueue;
    std::cout << "ThreadPool has been destroyed...\n";
}
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <queue>
#include <map>
#include <pthread.h>
#include <unistd.h>


// C++11多线程
class ThreadPool;
class TaskQueue;
class Task;


class ThreadPool {
public:
    ThreadPool() = default;
    ThreadPool(int maxNum, int minNum, int capacity);
    auto addTask(void (*func)(void *arg), void *arg) -> bool;
    ~ThreadPool();
private:
    pthread_t managerId;
    std::map<pthread_t, bool> workerMaps;
    static auto manager(void *arg) -> void*;
    static auto worker(void *arg) -> void*;
    auto takeTask() -> Task*;
    auto addWorker(pthread_t tid) -> bool;
    const int maxThreadNum;
    const int minThreadNum;
    bool isShut;
    bool kill;
    const int killNum;

    TaskQueue *taskQueue;

    pthread_mutex_t mutexPool;
    pthread_cond_t isFull, isEmpty;
};

class TaskQueue {
friend ThreadPool;
public:
    TaskQueue() = default;
    TaskQueue(int capacity);
    ~TaskQueue();
private:
    const int queueCap;
    int currNum;
    std::queue<Task*> queue;
};

class Task {
public:
    Task();
    Task(void (*func)(void *arg), void *arg);
    ~Task();
    void (*func)(void *arg);
    void *arg;
};
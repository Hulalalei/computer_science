#include <iostream>
#include <mutex>
#include <atomic>

//饿汉模式：定义类的时候创建单例对象，线程安全，多线程下使用
#if 0
class TaskQueue {
public:
	//外部提供公共接口
	static TaskQueue* getTaskQueue() {
		return m_taskQ;
	}
private:
	//无参和拷贝函数私有化
	TaskQueue() = default;
	TaskQueue(const TaskQueue& t) = default;
	TaskQueue& operator=(const TaskQueue& t) = default;
	
	//静态成员私有化,静态变量，类内声明，类外实现
	static TaskQueue* m_taskQ;
};
TaskQueue* TaskQueue::m_taskQ = new TaskQueue;
#endif

//懒汉模式：什么时候需要单例对象，在使用的时候再去创建对应的实例，线程不安全，单线程下使用
#if 0
class TaskQueue {
public:
	//外部提供公共接口
	static TaskQueue* getTaskQueue() {
        m_taskQ = new TaskQueue;
		return m_taskQ;
	}
private:
	//无参和拷贝函数私有化
	TaskQueue() = default;
	TaskQueue(const TaskQueue& t) = default;
	TaskQueue& operator=(const TaskQueue& t) = default;
	
	//静态成员私有化,静态变量，类内声明，类外实现
	static TaskQueue* m_taskQ;
};
TaskQueue* TaskQueue::m_taskQ = nullptr;

#else if 1
class TaskQueue {
public:
	//1、外部提供公共接口
	static TaskQueue* getTaskQueue01() {
        //双重检查锁定,底层有问题
        if (m_taskQ01 == nullptr) {
            m_mutex.lock();
            if (m_taskQ01 == nullptr) {
                m_taskQ01 = new TaskQueue;
            }
            m_mutex.unlock();
        }
		return m_taskQ01;
	}

    //2、原子变量+双重检查锁定, 按序处理指令
    static TaskQueue* getTaskQueue02() {
        TaskQueue* task = m_taskQ02.load();
        if (task == nullptr) {
            m_mutex.lock();
            task = m_taskQ02.load();
            if (task == nullptr) {
                task = new TaskQueue;
                m_taskQ02.store(task);
            }
            m_mutex.unlock();
        }
		return task;
    }

	//3、使用静态变量对象解决线程安全问题->编译器必须支持c++11
	static TaskQueue* getTaskQueue02() {
		static TaskQueue task;
		return &task;
	}
private:
	//无参和拷贝函数私有化
	TaskQueue() = default;
	TaskQueue(const TaskQueue& t) = default;
	TaskQueue& operator=(const TaskQueue& t) = default;
	
	//静态成员私有化,静态变量，类内声明，类外实现
	static TaskQueue* m_taskQ01;
    static std::mutex m_mutex;
    static std::atomic<TaskQueue*> m_taskQ02;
};
TaskQueue* TaskQueue::m_taskQ01 = nullptr;
std::atomic<TaskQueue*> TaskQueue::m_taskQ02;
#endif

int main () {
	TaskQueue* taskQ = TaskQueue::getTaskQueue01();
}
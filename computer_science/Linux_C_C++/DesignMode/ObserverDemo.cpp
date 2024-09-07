#include <iostream>
#include <vector>


class Observer;

//主题抽象类
class Subject {
public:
    virtual void attach(Observer* observer) = 0;
    virtual void detach(Observer* observer) = 0;
    virtual void notify() = 0;
};

//观察者抽象类
class Observer {
public:
    virtual void update() = 0;
};


//具体观察者类A
class ConcreteObserverA : public Observer {
public:
    void update() override {
        std::cout << "具体观察者A收到通知并作出响应" << std::endl;
    }
};


//具体观察者类B
class ConcreteObserverB : public Observer {
public:
    void update() override {
        std::cout << "具体观察者B收到通知并作出响应" << std::endl;
    }
};

//具体主题类
class ConcreteSubject : public Subject {
public:
    void attach(Observer* observer) override {
        observers.push_back(observer);
    }
    void detach(Observer* observer) override {
        for (auto it = observers.begin(); it != observers.end(); it++) {
            if (*it == observer) {
                observers.erase(it);
                break;
            }
        }
    }
    void notify() override {
        for (auto observer : observers) {
            observer->update();
        }
    }
private:
    std::vector<Observer*> observers;
};

int main(void) {
    ConcreteSubject subject;

    Observer* observerA = new ConcreteObserverA();
    Observer* observerB = new ConcreteObserverB();

    subject.attach(observerA);
    subject.attach(observerB);

    subject.notify();

    subject.detach(observerA);

    subject.notify();

    delete observerA;
    delete observerB;
    
    return 0;
}
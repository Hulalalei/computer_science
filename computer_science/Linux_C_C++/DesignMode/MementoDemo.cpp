#include <iostream>
#include <string>

//备忘录类
class Memento {
public:
    Memento(const std::string& state): state(state) {}

    std::string getState() const {
        return state;
    }
private:
    std::string state;
};

//原发器
class Originator {
public:
    void setState(const std::string& state) {
        this->state = state;
    }

    std::string getState() const {
        return state;
    }

    Memento* createMemento() const {
        return new Memento(state);
    }

    void restoreMemento(const Memento* memento) {
        state = memento->getState();
    }
private:
    std::string state;
};

//负责人类
class Caretaker {
public:
    void saveMemento(Memento* memento) {
        this->memento = memento;
    }

    Memento* getMemento() const {
        return memento;
    }
private:
    Memento* memento;
};

int main(void) {
    Originator originator;
    Caretaker caretaker;

    //设置初始状态
    originator.setState("state 1");
    std::cout << "Current state: " << originator.getState() << std::endl;

    //保存备忘录
    caretaker.saveMemento(originator.createMemento());

    //修改状态
    originator.setState("state 2");
    std::cout << "Current state: " << originator.getState() << std::endl;

    //恢复到原来状态
    originator.restoreMemento(caretaker.getMemento());
    std::cout << "Current state: " << originator.getState() << std::endl;

    return 0;
}
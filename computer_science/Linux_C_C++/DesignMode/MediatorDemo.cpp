#include <iostream>
#include <algorithm>
#include <string>

class Colleague;

//中介者抽象类
class Mediator {
public:
    virtual void sendMessage(const std::string& message, Colleague* colleague) = 0;
};


//同事类
class Colleague {
public:
    void setMediator(Mediator* mediator) {
        this->mediator = mediator;
    }

    virtual void sendMessage(const std::string& message) = 0;
    virtual void receiveMessage(const std::string& message) = 0;
protected:
    Mediator* mediator;
};


//具体实现类A
class ConcreteColleagueA : public Colleague {
public:
    void sendMessage(const std::string& message) override {
        mediator->sendMessage(message, this);
    }
    void receiveMessage(const std::string& message) override {
        std::cout << "同事类A收到消息：" << message << std::endl;
    }
};

//具体实现类B
class ConcreteColleagueB : public Colleague {
public:
    void sendMessage(const std::string& message) override {
        mediator->sendMessage(message, this);
    }
    void receiveMessage(const std::string& message) override {
        std::cout << "同事类B收到消息：" << message << std::endl;
    }
};

//具体中介者类
class ConcreteMediator : public Mediator{
public:
    void setColleague1(Colleague* colleague) {
        colleague1 = colleague;
    }
    void setColleague2(Colleague* colleague) {
        colleague2 = colleague;
    }
    void sendMessage(const std::string& message, Colleague* colleague) override {
        if (colleague == colleague1) {
            colleague2->receiveMessage(message);
        }
        else if (colleague == colleague2) {
            colleague1->receiveMessage(message);
        }
    }
private:
    Colleague* colleague1;
    Colleague* colleague2;
};



int main(void) {
    ConcreteMediator* mediator = new ConcreteMediator();

    Colleague* colleague1 = new ConcreteColleagueA();
    Colleague* colleague2 = new ConcreteColleagueB();

    mediator->setColleague1(colleague1);
    mediator->setColleague2(colleague2);

    colleague1->setMediator(mediator);
    colleague2->setMediator(mediator);

    colleague1->sendMessage("hello, colleagueB!");
    colleague2->sendMessage("hello, colleagueA!");

    delete mediator;
    delete colleague1;
    delete colleague2;
    
    return 0;
}
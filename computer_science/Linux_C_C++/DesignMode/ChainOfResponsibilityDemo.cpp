#include <iostream>
#include <algorithm>


//抽象处理器
class Handle {
public:
    Handle(): nextHandler(nullptr) {}
    void setNextHandler(Handle* handler) {
        nextHandler = handler;
    }

    virtual void handleRequest(int request) = 0;
protected:
    Handle* nextHandler;
};


//具体处理器A
class ConcreteHandlerA : public Handle {
public:
    void handleRequest(int request) override {
        if (request >= 0 && request < 10) {
            std::cout << "具体处理器A处理请求：" << request << std::endl;
        }
        else if (nextHandler != nullptr) {
            nextHandler->handleRequest(request);
        }
    }
};


//具体处理器B
class ConcreteHandlerB : public Handle {
public:
    void handleRequest(int request) override {
        if (request >= 10 && request < 20) {
            std::cout << "具体处理器B处理请求：" << request << std::endl;
        }
        else if (nextHandler != nullptr) {
            nextHandler->handleRequest(request);
        }
    }
};


//具体处理器C
class ConcreteHandlerC : public Handle {
public:
    void handleRequest(int request) override {
        if (request >= 20 && request < 30) {
            std::cout << "具体处理器C处理请求：" << request << std::endl;
        }
        else if (nextHandler != nullptr) {
            nextHandler->handleRequest(request);
        }
    }
};


int main(void) {
    Handle* handlerA = new ConcreteHandlerA();
    Handle* handlerB = new ConcreteHandlerB();
    Handle* handlerC = new ConcreteHandlerC();

    handlerA->setNextHandler(handlerB);
    handlerB->setNextHandler(handlerC);

    handlerA->handleRequest(5);
    handlerA->handleRequest(15);
    handlerA->handleRequest(25);

    delete handlerA;
    delete handlerB;
    delete handlerC;

    return 0;
}
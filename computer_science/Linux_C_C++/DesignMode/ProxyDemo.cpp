#include <iostream>
#include <algorithm>

//抽象主题
class Subject {
public:
    virtual void request() = 0;
};

//真实主题
class RealSubject : public Subject {
public:
    void request() override {
        std::cout << "真实主题的需求" << std::endl;
    }
};

//代理
class Proxy : public Subject {
public:
    Proxy() {
        realSubject = new RealSubject();
    }
    ~Proxy() {
        delete realSubject;
    }
    void request() override {
        std::cout << "代理的请求" << std::endl;
        realSubject->request();
    }
private:
    RealSubject* realSubject;
};

int main (void) {
    Subject* proxy = new Proxy();
    proxy->request();

    delete proxy;

    return 0;
}
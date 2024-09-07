#include <iostream>


//目标接口
class Target {
public:
    virtual void request() = 0;
};

//被适配者类
class Adaptee {
public:
    void specificRequest() {
        std::cout << "被适配者的特殊请求" << std::endl;
    }
};

//适配器
class Adapter : public Target, private Adaptee{
public:
    void request() override {
        specificRequest();
    }
};

int main (void) {
    Target* adapter = new Adapter();
    adapter->request();

    delete adapter;
    return 0;
}
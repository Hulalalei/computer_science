#include <iostream>
#include <algorithm>
#include <vector>
#include <unordered_map>

//抽象享元类
class FlyWeight {
public:
    virtual void operation(int extrinsicState) = 0;
};

//具体享元类
class ConcreteFlyWeight : public FlyWeight {
public:
    void operation(int extrisicState) override {
        std::cout << "外部状态：" << extrisicState << std::endl;
    }
};

//享元工厂类，类似于池子
class FlyWeightFactory {
public:
    FlyWeight* getFlyWeight(int key) {
        if (flyweight.find(key) == flyweight.end()) {
            flyweight[key] = new ConcreteFlyWeight();
        }

        return flyweight[key];
    }
private:
    std::unordered_map<int, FlyWeight*> flyweight;
};

int main (void) {
    FlyWeightFactory factory;

    FlyWeight* flyweight = factory.getFlyWeight(1); //内部状态，容器是共享资源
    flyweight->operation(100);  //外部状态

    flyweight = factory.getFlyWeight(2);
    flyweight->operation(200);

    delete flyweight;

    return 0;
}
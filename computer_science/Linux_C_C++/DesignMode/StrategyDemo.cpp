#include <iostream>

//策略抽象类
class Strategy {
public:
    virtual void execute() = 0;
};


//具体策略类A
class ConcreteStrategyA : public Strategy {
public:
    void execute() override {
        std::cout << "使用策略A执行算法" << std::endl;
    }
};

//具体策略类B
class ConcreteStrategyB : public Strategy {
public:
    void execute() override {
        std::cout << "使用策略B执行算法" << std::endl;
    }
};

//上下文类
class Context {
public:
    void setStrategy(Strategy* strategy) {
        this->strategy = strategy;
    }

    void executeStrategy() {
        if (strategy) {
            strategy->execute();
        }
    }
private:
    Strategy* strategy;
};

int main(void) {
    Context* context;

    //使用策略A执行算法
    ConcreteStrategyA strategyA;
    context->setStrategy(&strategyA);
    context->executeStrategy();

    //使用策略A执行算法
    ConcreteStrategyB strategyB;
    context->setStrategy(&strategyB);
    context->executeStrategy();

    delete context;
    return 0;
}
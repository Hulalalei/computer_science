#include <iostream>

class Context;

//状态抽象类
class State {
public:
    virtual void handle(Context* context) = 0;
};


//具体状态A
class ConcreteStateA : public State {
public:
    void handle(Context* context) override;
};


//具体状态B
class ConcreteStateB : public State {
public:
    void handle(Context* context) override;
};


//上下文类
class Context {
public:
    Context(State* initialState): currentState(initialState) {}
    void setState(State* state) {
        currentState = state;
    }
    void request() {
        currentState->handle(this);
    }
private:
    State* currentState;
};

void ConcreteStateA::handle(Context* context) {
    std::cout << "当前状态是A，执行操作A，切换到状态B" << std::endl;
    context->setState(new ConcreteStateB());
}

void ConcreteStateB::handle(Context* context) {
    std::cout << "当前状态是B，执行操作B，切换到状态A" << std::endl;
    context->setState(new ConcreteStateA());
}

int main(void) {
    Context* context = new Context(new ConcreteStateA());

    context->request();
    context->request();

    delete context;

    return 0;
}
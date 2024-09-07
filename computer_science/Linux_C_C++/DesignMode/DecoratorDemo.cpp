#include <iostream>
#include <algorithm>


//抽象构件
class Component {
public:
	virtual void operation() = 0;
};

//具体构件
class ConcreteComponent : public Component {
public:
	void operation() override {
    	std::cout << "具体构件操作" << std::endl;
    }
};

//抽象装饰者
class Decorator : public Component {
public:
	Decorator(Component* component) : m_component(component) {}
    void operation() override {
    	if (m_component != nullptr) {
        	m_component->operation();
        }
    }
protected:
	Component* m_component;
};

//具体装饰者A
class ConcreteDecoratorA : public Decorator {
public:
	ConcreteDecoratorA(Component* component) : Decorator(component) {}
	void operation() override {
    	addedBehavior();
        Decorator::operation();
    }
    
    void addedBehavior() {
    	std::cout << "具体装饰者A的操作" << std::endl;
    }
};

//具体装饰者B
class ConcreteDecoratorB : public Decorator {
public:
	ConcreteDecoratorB(Component* component) : Decorator(component) {}
	void operation() override {
    	addedBehavior();
        Decorator::operation();
    }
    
    void addedBehavior() {
    	std::cout << "具体装饰者B的操作" << std::endl;
    }
};


int main(void) {
	Component* component = new ConcreteComponent();
    
	Component* decoratorA = new ConcreteDecoratorA(component);
	decoratorA->operation();
	
    Component* decoratorB = new ConcreteDecoratorB(component);
	decoratorB->operation();
	return 0;
}
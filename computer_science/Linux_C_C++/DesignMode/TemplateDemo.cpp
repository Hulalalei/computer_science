#include <iostream>
#include <algorithm>


//实现部分
class Implementor {
public:
	virtual void operationImpl() = 0;
};


//具体实现部分A
class ConcreteImplementorA : public Implementor {
public:
	void operationImpl() override {
    	std::cout << "具体实现A的操作" << std::endl;
    }
};


//具体实现部分B
class ConcreteImplementorB : public Implementor {
public:
	void operationImpl() override {
    	std::cout << "具体实现B的操作" << std::endl;
    }
};


//抽象部分
class Abstraction {
public:
	Abstraction(Implementor* implementor): m_implementor(implementor) {}
    virtual void operation() = 0;
protected:
	Implementor* m_implementor;
};


//具体抽象部分A
class ConcreteAbstractionA : public Abstraction {
public:
	ConcreteAbstractionA(Implementor* implementor): Abstraction(implementor) {}
    void operation() override {
    	std::cout << "抽象部分A的具体操作" << std::endl;
        m_implementor->operationImpl();
    }
};


//具体抽象部分B
class ConcreteAbstractionB : public Abstraction {
public:
	ConcreteAbstractionB(Implementor* implementor): Abstraction(implementor) {}
    void operation() override {
    	std::cout << "抽象部分B的具体操作" << std::endl;
        m_implementor->operationImpl();
    }
};



int main(void) {
	Implementor* implementorA = new ConcreteImplementorA();
	Abstraction* abstractionA = new ConcreteAbstractionA(implementorA);
    abstractionA->operation();
    
    Implementor* implementorB = new ConcreteImplementorB();
	Abstraction* abstractionB = new ConcreteAbstractionB(implementorB);
    abstractionB->operation();
    
    
    delete implementorA;
    delete abstractionA;
    delete implementorB;
    delete abstractionB;

	return 0;
}
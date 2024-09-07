#include <iostream>
#include <vector>


//前向声明
class ConcreteElementA;
class ConcreteElementB;


//访问者接口
class Visitor {
public:
	virtual void visit(ConcreteElementA* element) = 0;
    virtual void visit(ConcreteElementB* element) = 0;
};


//具体访问者A
class ConcreteVisitorA : public Visitor {
public:
	void visit(ConcreteElementA* element) override {
    	std::cout << "具体访问者A访问具体元素A，执行操作A" << std::endl;
    }
    void visit(ConcreteElementB* element) override {
    	std::cout << "具体访问者A访问具体元素B，执行操作B" << std::endl;
    }
};


//具体访问者B
class ConcreteVisitorB : public Visitor {
public:
	void visit(ConcreteElementA* element) override {
    	std::cout << "具体访问者B访问具体元素A，执行操作A" << std::endl;
    }
    void visit(ConcreteElementB* element) override {
    	std::cout << "具体访问者B访问具体元素B，执行操作B" << std::endl;
    }
};


//元素接口
class Element {
public:
	virtual void accept(Visitor* visitor) = 0;
};


//具体元素A
class ConcreteElementA : public Element{
public:
	void accept(Visitor* visitor) override {
    	visitor->visit(this);
    }
};


//具体元素B
class ConcreteElementB : public Element{
public:
	void accept(Visitor* visitor) override {
    	visitor->visit(this);
    }
};


//对象结构
class ObjectStructure {
public:
	void addElement(Element* element) {
    	elements.push_back(element);
    }
    void removeElement(Element* element) {
    	elements.pop_back();
    }
    
    void accept(Visitor* visitor) {
    	for (auto element : elements) {
        	element->accept(visitor);
        }
    }
private:
	std::vector<Element*> elements;
};

int main(void) {
	ObjectStructure objectStructure;
    objectStructure.addElement(new ConcreteElementA());
	objectStructure.addElement(new ConcreteElementB());
    
    
    Visitor* visitorA = new ConcreteVisitorA();
    Visitor* visitorB = new ConcreteVisitorB();
    
    objectStructure.accept(visitorA);
    objectStructure.accept(visitorB);
    
    delete visitorA;
    delete visitorB;
    
	return 0;
}
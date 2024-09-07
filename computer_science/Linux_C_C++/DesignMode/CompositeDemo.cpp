#include <iostream>
#include <algorithm>
#include <vector>


//抽象组件类
class Component {
public:
    virtual void operation() = 0;
};

//叶子组件类
class Leaf : public Component {
public:
    void operation() override {
        std::cout << "叶子组件操作" << std::endl;
    }
};

//组合组件类
class Composite : public Component {
public:
    void addComponent(Component* component) {
        components.push_back(component);
    }
    void operation() override {
        std::cout << "组合组件的操作" << std::endl;
        for (Component* component : components) {
            component->operation();
        }
    }
private:
    std::vector<Component*> components;
};


int main (void) {
    Component* leaf = new Leaf();
    Composite* composite = new Composite();
    
    composite->addComponent(leaf);
    composite->operation();

    return 0;
}
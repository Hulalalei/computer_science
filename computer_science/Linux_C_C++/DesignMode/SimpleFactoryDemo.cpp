#include <iostream>
#include <string>

class AbstractSmell {
public:
    virtual void ability() = 0;
    virtual void transform() = 0;
    virtual ~AbstractSmell() {}
};

class SheepSmell : public AbstractSmell {
public:
    void ability() override {
        std::cout << "炖羊肉...\n";
    }

    void transform() override {
        std::cout << "变山羊...\n";
    }
};

class LionSmell : public AbstractSmell {
public:
    void ability() override {
        std::cout << "炖狮子...\n";
    }

    void transform() override {
        std::cout << "变狮子...\n";
    }
};

class BatSmell : public AbstractSmell {
public:
    void ability() override {
        std::cout << "炖蝙蝠...\n";
    }
    void transform() override {
        std::cout << "变蝙蝠...\n";
    }
};

//c++11枚举强类型
enum class Type : char {sheep, lion, bat};
class SmellFactory {
public:
    AbstractSmell* createSmell(Type type) {
        AbstractSmell* ptr = nullptr;
        switch (type) {
            case Type::sheep :
            ptr = new SheepSmell;
            break;

            case Type::lion :
            ptr = new LionSmell;
            break;

            case Type::bat :
            ptr = new BatSmell;
            break;

            default : break; 
        }
        return ptr;
    }
};

int main (void* arg) {
    SmellFactory* factory = new SmellFactory;
    AbstractSmell* ptr = factory->createSmell(Type::bat);
    ptr->ability();
    ptr->transform();

    delete factory;
    delete ptr;

    return 0;
}
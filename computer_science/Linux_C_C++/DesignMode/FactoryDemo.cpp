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

////////////////////////////////////////////////////////////

class AbstractFactory {
public:
    virtual AbstractSmell* createSmell() = 0;
    virtual ~AbstractFactory() {}
};

class SheepFactory : public AbstractFactory{
public:
    AbstractSmell* createSmell() override {
        return new SheepSmell;
    }
};

class LionFactory : public AbstractFactory{
public:
    AbstractSmell* createSmell() override {
        return new LionSmell;
    }
};

class BatFactory : public AbstractFactory{
public:
    AbstractSmell* createSmell() override {
        return new BatSmell;
    }
};

int main (void* arg) {
    AbstractFactory* factory = new BatFactory;
    AbstractSmell* ptr = factory->createSmell();
    ptr->ability();
    ptr->transform();

    delete factory;
    delete ptr;

    return 0;
}
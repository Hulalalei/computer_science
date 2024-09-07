#include <iostream>


class Base {
public:
    virtual ~Base() = 0;
};

Base::~Base() {
    std::cout << "Base destructor called" << std::endl;
}

class Derived : public Base {
public:
   
};


int main() {
    Derived d;


}
#include <iostream>
#include <algorithm>

//产品类
class Pizza {
public:
    void setDough(const std::string& dough) {
        m_dough = dough;
    }
    void setSauce(const std::string& sauce) {
        m_sauce = sauce;
    }
    void setTopping(const std::string& topping) {
        m_topping = topping;
    }
    void showPizza() const {
        std::cout << "dough = " << m_dough << " sauce = " << m_sauce << " topping = " << m_topping << std::endl;
    }
private:
    std::string m_dough;
    std::string m_sauce;
    std::string m_topping;
};

//抽象建造者类
class PizzaBuilder {
public:
    virtual ~PizzaBuilder() {}
    virtual void buildDough() = 0;
    virtual void buildSauce() = 0;
    virtual void buildTopping() = 0;
    virtual Pizza* getPizza() = 0;
};

//具体建造类A
class HawaiianPizzaBuilder : public PizzaBuilder {
public:
    void buildDough() override {
        m_pizza->setDough("cross");
    }
    void buildSauce() override {
        m_pizza->setSauce("mild");
    }
    void buildTopping() override {
        m_pizza->setTopping("ham and pineapple");
    }
    Pizza* getPizza() override {
        return m_pizza;
    }
private:
    Pizza* m_pizza = new Pizza();
};

//具体建造类B
class SpicyPizzaBuilder : public PizzaBuilder {
public:
    void buildDough() override {
        m_pizza->setDough("pan baked");
    }
    void buildSauce() override {
        m_pizza->setSauce("hot");
    }
    void buildTopping() override {
        m_pizza->setTopping("pepperoni and jalapeno");
    }
    Pizza* getPizza() override {
        return m_pizza;
    }
private:
    Pizza* m_pizza = new Pizza();
};

//指导者类
class PizzaDirector {
public:
    void setPizzaBuilder(PizzaBuilder* builder) {
        m_builder = builder;
    }
    void constructPizza() {
        m_builder->buildDough();
        m_builder->buildSauce();
        m_builder->buildTopping();
    }
private:
    PizzaBuilder* m_builder;
};

int main (void) {
    PizzaDirector director;

    HawaiianPizzaBuilder hawaiianBuilder;
    director.setPizzaBuilder(&hawaiianBuilder);
    director.constructPizza();
    Pizza* hawaiianPizza = hawaiianBuilder.getPizza();
    hawaiianPizza->showPizza();

    return 0;
}
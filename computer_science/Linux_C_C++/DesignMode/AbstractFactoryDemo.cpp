#include <iostream>

//部件1、船身
class ShipBody {
public:
    virtual std::string getBody() = 0;
    virtual ~ShipBody() {}
};

class WoodenBody : public ShipBody {
public:
    std::string getBody() override {
        return std::string("使用<木材>制作船体...\n");
    }
};

class IronBody : public ShipBody {
public:
    std::string getBody() override {
        return std::string("使用<钢铁>制作船体...\n");
    }
};

class MentalBody : public ShipBody {
public:
    std::string getBody() override {
        return std::string("使用<合金>制作船体...\n");
    }
};

//部件2、引擎
class Engine {
public:
    virtual std::string getEngine() = 0;
    virtual ~Engine() {}
};

class Human : public Engine {
public:
    std::string getEngine() override {
        return std::string("船的动力来源：<手动>...\n");
    }
};

class Diesel : public Engine {
public:
    std::string getEngine() override {
        return std::string("船的动力来源：<内燃机>...\n");
    }
};

class Nuclear : public Engine {
public:
    std::string getEngine() override {
        return std::string("船的动力来源：<原子核>...\n");
    }
};

//部件3、武器
class Weapon {
public:
    virtual std::string getWeapon() = 0;
    virtual ~Weapon() {}
};

class Gun : public Weapon {
public:
    std::string getWeapon() override {
        return std::string("船中提供的武器是：<枪>...\n");
    }
};

class Laser : public Weapon {
public:
    std::string getWeapon() override {
        return std::string("船中提供的武器是：<镭射线>...\n");
    }
};

class Cannon : public Weapon {
public:    
    std::string getWeapon() override {
        return std::string("船中提供的武器是：<加农炮>...\n");
    }
};

//总体架构、船
class Ship {
public:
    Ship(ShipBody* body, Engine* engine, Weapon* weapon) : 
        m_body(body), m_engine(engine), m_weapon(weapon) {};
    
    ~Ship() {
        delete m_body;
        delete m_engine;
        delete m_weapon;
    }

    std::string getProperty() {
        std::string info = m_body->getBody() + m_engine->getEngine() + m_weapon->getWeapon();
        return info;
    }
private:
    ShipBody* m_body;
    Engine* m_engine;
    Weapon* m_weapon;
};

//工厂类---抽象类
class AbstractFactory {
public:
    virtual Ship* createShip() = 0;
    virtual ~AbstractFactory() {}
};

class BasicFactory : public AbstractFactory {
public:
    Ship* createShip() override {
        Ship* ship = new Ship(new WoodenBody, new Human, new Gun);
        return ship;
    }
};

class StandardFactory : public AbstractFactory {
public:
    Ship* createShip() override {
        Ship* ship = new Ship(new IronBody, new Diesel, new Laser);
        return ship;
    }
};

class UltimateFactory : public AbstractFactory {
public:
    Ship* createShip() override {
        Ship* ship = new Ship(new MentalBody, new Nuclear, new Cannon);
        return ship;
    }
};

int main(void) {
    AbstractFactory* factory = new UltimateFactory;
    Ship* ship = factory->createShip(); 
    std::cout << ship->getProperty();

    delete ship;
    delete factory;

    return 0;
}
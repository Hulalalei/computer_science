#include <iostream>
#include <algorithm>

//圆形基类
class Shape {
public:
    virtual ~Shape() {}
    virtual void draw() = 0;
    virtual Shape* clone() = 0;
};

//圆形类
class Circle : public Shape {
public:
    Circle(std::string color, int radius) : m_color(color), m_radius(radius) {}
    void draw() override {
        std::cout << "color = " << m_color << " radius = " << m_radius << std::endl;
    }
    Shape* clone() override {
        return new Circle(*this);
    }
private:
    std::string m_color;
    int m_radius;
};

int main (void) {
    Shape* circlePrototype = new Circle("red", 10);
    Shape* circle1 = circlePrototype->clone();
    Shape* circle2 = circlePrototype->clone();

    circle1->draw();
    circle2->draw();

    delete circle1, circle2, circlePrototype;
    return 0;
}
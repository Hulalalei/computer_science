module;

// 全局代码段
#include <iostream>
#define PI 3.1415926

export module math;


export template<class T>
T add(T a, T b) {
    return a + b;
}

export double circle_area(double r) {
    return PI * r * r;
}

// 私有代码段
module : private;

template <class T>
T multiply(T a, T b) {
    return a * b;
}

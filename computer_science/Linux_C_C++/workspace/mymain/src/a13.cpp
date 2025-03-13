#include <cstdio>
#include <iostream>



int main() {
    int a = 10, b = 20;
    (a = 0) || (b = 2);
    std::cout << a << " " << b;

    return 0;
}

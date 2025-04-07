#include <cstdio>
#include <iostream>
#include <ostream>

int gcd(int a, int b) {
    return b ? gcd(b, a % b) : a;
}

int main() {
    // gcd * lcm == a * b

    return 0;
}

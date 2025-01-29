module;
// #include <iostream>

export module sum;
import <iostream>;

export int sums(int a, int b) {
    std::cout << "ok!\n";
    return a + b;
}

export namespace t1 {
    int a1() { return 1; }
    int a2() { return 2; }
}

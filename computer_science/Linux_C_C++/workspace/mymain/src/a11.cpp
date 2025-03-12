#include <cstdio>
#include <iostream>


int main() {
    int spaces = 3, spots = 1;
    for (int i = 0; i < 4; i ++) {
        for (int j = 0; j < spaces; j ++) {
            std::cout << " ";
        }
        spaces -= 1;
        for (int k = 0; k < spots; k ++) {
            std::cout << "*";
        }
        spots += 2;
        std::cout << std::endl;
    }

    spaces = 1, spots = 5;
    for (int i = 0; i < 3; i ++) {
        for (int j = 0; j < spaces; j ++) {
            std::cout << " ";
        }
        spaces += 1;
        for (int k = 0; k < spots; k ++) {
            std::cout << "*";
        }
        spots -= 2;
        std::cout << std::endl;
    }
    return 0;
}

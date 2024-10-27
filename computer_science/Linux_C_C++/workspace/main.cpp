#include <iostream>
#include <format>



int son[1024][2] = { 0 };

int solve(int sub) {
    int height = 0;
    int total = 0;
    while (true) {
        if (son[sub][0] != -1) {
            height ++;
            solve(son[sub][0]);
            height --;
        }
        if (son[sub][1] != -1) {
            height ++;
            solve(son[sub][1]);
            height --;
        }
        if (son[sub][0] == -1 && son[sub][1] == -1) {
            total += height * son[sub].weight;
        }
    }
    return total;
}


int main(int argc, char **argv) {
    std::cout << solve(0) << "\n";

    return 0;
}

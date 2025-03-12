#include <iostream>

void solve(int n) {
    int col = n;
    int num = 0;
    for (int i = 0; i < n; i ++) {
        for (int j = 0; j < col; j ++) {
            std::cout << (num ++) % 10 << " ";
        }
        std::cout << std::endl;
        col --;
    }
}

int main() {
    solve(7);
}

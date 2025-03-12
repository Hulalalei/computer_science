#include <iostream>
#include <cstring>


int n = 0;
int triangle[32][32];

int main() {
    std::cin >> n;
    memset(triangle, 0, sizeof(triangle));
    triangle[0][0] = 1;
    triangle[1][0] = 1;
    triangle[1][1] = 1;

    for (int i = 2; i <= n; i ++) {
        triangle[i][0] = 1;
        triangle[i][i] = 1;

        for (int j = 1; j <= i; j ++) {
            triangle[i][j] = triangle[i - 1][j] + triangle[i - 1][j - 1];
        }
    }

    for (int i = 0; i <= n; i ++) {
        for (int j = 0; j <= n; j ++) {
            if (triangle[i][j] != 0) std::cout << triangle[i][j] << " ";
        }
    }
}

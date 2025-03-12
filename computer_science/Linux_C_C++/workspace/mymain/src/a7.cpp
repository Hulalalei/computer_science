

#include <iostream>
#include <ostream>
int k = 0, n = 0;
int flag[1000] = {};

int main() {
    std::cin >> k >> n;
    for (int i = 1; i <= k; i ++) {
        for (int j = 1; j <= n; j ++) {
            if (j % i == 0) {
                if (flag[j]) flag[j] = false;
                else flag[j] = true;
            }
        }
    }

    for (int i = 1; i <= n; i ++)
        if (flag[i]) std::cout << i << std::endl;
}

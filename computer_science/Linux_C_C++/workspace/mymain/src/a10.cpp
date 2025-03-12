#include <iostream>
#include <algorithm>
#include <cstdio>


int main() {
    int i = 1;
    float tmp = 1.0, ans = 0.0;
    while (tmp > 1e-4) {
        tmp = (float)(2 * i - 1) / ((2 * i) * (2 * i));
        ans += tmp;
        i ++;
    }
    std::cout << tmp << std::endl;
    std::cout << ans;

    return 0;
}

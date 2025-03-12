

#include <iostream>
int lags = 0, cnt = 0;
// 设鸡为x个，兔为y个
// 2x + 4y = lags
// x + y = cnt   ->   y = cnt - x
// 整合后：4cnt - 2x = lags;

int main() {
    std::cin >> lags >> cnt;
    int tmp = 4 * cnt - lags;

    if (tmp % 2 == 0) std::cout << tmp / 2 << " " << cnt - tmp / 2;
    else std::cout << "No answer\n";
}

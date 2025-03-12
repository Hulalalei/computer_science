#include <iostream>
#include <ostream>


int k = 0, n = 0;

int main() {
    std::cin >> k >> n;
    int total = 2 * n;
    int ans = 0;
    if (total < k) {
        std::cout << "2\n";
    } else {
        ans = total / k;
        if (total % k != 0) ans ++;
    }
    std::cout << ans << std::endl;
    return 0;
}

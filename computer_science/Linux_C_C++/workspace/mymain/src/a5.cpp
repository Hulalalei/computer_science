#include <iostream>
#include <ostream>
#include <string>


int dfs(int const &m, int h = 0) {
    if (h == m) return 1;
    return (dfs(m, h + 1) + 1) * 2;
}

int main() {
    int m = 0;
    std::cin >> m;
    std::cout << dfs(m) << std::endl;

    int result = 1;
    for (int i = 1; i <= m; i ++) {
        result = (result + 1) * 2;
    }
    std::cout << result << std::endl;
}

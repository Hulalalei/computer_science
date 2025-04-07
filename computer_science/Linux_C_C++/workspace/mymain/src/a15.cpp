#include <iostream>


int dfs(int h) {
    if (h == 0) return 1;
    return (dfs(h - 1) + 1) * 2;
}

int main() {
    int m = 0;
    std::cin >> m;
    std::cout << dfs(m);

    return 0;
}

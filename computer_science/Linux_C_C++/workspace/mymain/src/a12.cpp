#include <iostream>
#include <vector>


int ans = 0x3f;
int a[5] = {1, 8, -1, 4, -2};
int b[5] = {0, 6, 1, -4, -1};
bool st[5] = {};
std::vector<int> path;

void dfs(int index) {
    if (index == 5) {
        int tmp = 0;
        for (auto v : path)
            tmp += v;
        ans = std::min(tmp, ans);
        return;
    }
    for (int j = 0; j < 5; j ++) {
        if (!st[j]) {
            st[j] = true;
            path.emplace_back(a[index] * b[j]);
            dfs(index + 1);
            path.pop_back();
            st[j] = false;
        }
    }
}

int main() {
    dfs(0);
    std::cout << ans << std::endl;
}

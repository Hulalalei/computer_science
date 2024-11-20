#include <iostream>
#include <format>
#include <cstring>
#include <algorithm>


int n = 0, m = 0;
constexpr int N = 1000;
int g[N][N], dist[N];
bool st[N];

int dijkstra() {
    dist[1] = 0;

    for (int i = 0; i < n; i ++) {
        int t = -1;
        for (int j = 1; j <= n; j ++) 
            if (!st[j] && (t == -1 || dist[t] > dist[j]))
                t = j;
        st[t] = true;
        for (int j = 1; j <= n; j ++) 
            dist[j] = std::min(dist[j], dist[t] + g[t][j]);
    }

    if (dist[n] == 0x3f3f3f3f) return -1;
    return dist[n];
}

int main(int argc, char **argv) {
    memset(g, 0x3f, sizeof g);
    memset(dist, 0x3f, sizeof dist);

    std::cin >> n >> m;
    while (m --) {
        int x = 0, y = 0, z = 0;
        std::cin >> x >> y >> z;
        g[x][y] = std::min(g[x][y], z);
    }
    std::cout << dijkstra() << "\n";
    return 0;
}

#include <iostream>
#include <algorithm>
#include <cstring>


constexpr int N = 1e3;
int h[N], e[N], ne[N], w[N], idx = 0;
int dist[N], path[N], tt = -1;
bool st[N];

void addplus(int x, int y, int z) {
    e[idx] = x, ne[idx] = h[y], h[y] = idx, w[idx ++] = z;
    e[idx] = y, ne[idx] = h[x], h[x] = idx, w[idx ++] = z;
}

void create() {
    addplus(1, 2, 7);
    addplus(1, 3, 11);
    addplus(2, 3, 10);
    addplus(2, 4, 9);
    addplus(3, 4, 5);
    addplus(3, 5, 7);
    addplus(3, 6, 8);
    addplus(5, 6, 6);
}

int dijkstra(int x = 1) {
    dist[x] = 0;
    
    for (int i = 0; i < 6; i ++) {
        int t = -1;
        for (int j = 1; j <= 6; j ++) {
            if (!st[j] && (t == -1 || dist[t] > dist[j]))
                t = j;
        }
        
        st[t] = true;
        path[++ tt] = t;

        for (int j = 1; j <= 6; j ++) {
            int weight = 0x3f3f3f3f;
            for (int k = h[t]; k != -1; k = ne[k]) {
                if (e[k] == j) {
                    weight = w[k];
                    break;
                }
            }
            dist[j] = std::min(dist[j], dist[t] + weight);
        }
    }

    return dist[6];
}

int main(int argc, char **argv) {
    memset(h, -1, sizeof h);
    memset(dist, 0x3f, sizeof dist);

    create();
    std::cout << dijkstra() << "\n";
    for (int i = 0; i < 6; i ++) std::cout << path[i] << " ";
    std::cout << "\n";

    return 0;
}
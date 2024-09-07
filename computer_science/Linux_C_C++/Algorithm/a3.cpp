#include <iostream>
#include <algorithm>
#include <cstring>



// 邻接表转换为邻接矩阵
constexpr int N = 1e3;
int h[N], e[N], ne[N], path[N];
int idx = 0, t = 0;
bool v[N];

void add(int a, int b);
void AdjGraph();
void readMap();
void dfs(int i, int j);

int main(int argc, char **argv) {
    memset(path, -1, sizeof(path));
    memset(h, -1, sizeof(h));
    
    AdjGraph();
    // readMap();
    dfs(1, 2);

    return 0;
}

void add(int a, int b) {
    e[idx] = b, ne[idx] = h[a], h[a] = idx ++;
}

void AdjGraph() {
    int temp = 1;
    for (int i = 0; i < 3; i ++) {
        add((temp + i) % 3, (temp + i + 1) % 3);
        add((temp + i + 1) % 3, (temp + i) % 3);
    }
}

void readMap() {
    for (int i = 0; i < 3; i ++) {
        std::cout << i << " ";
        for (int j = h[i]; j != -1; j = ne[j]) {
            std::cout << e[j] << " ";
        }
        std::cout << "\n";
    }
}


// 从0到2的所有路径
void dfs(int i, int j) {
    path[++ t] = i;
    v[i] = true;

    for (int k = h[i]; k != -1 && !v[e[h[i]]]; k = ne[k]) {
        dfs(e[h[i]], j);
        v[e[h[i]]] = false;
        -- t;
    }
    
    if (i == j) {
        for (int i = 0; i <= t; i ++) std::cout << path[i] << " ";
        std::cout << "\n";
    }
}
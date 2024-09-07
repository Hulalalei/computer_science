#include <iostream>
#include <algorithm>
#include <cstring>


constexpr int N = 128;
// bfs
int nodes = 7;
int g[N][N], path[N], pp = -1;
bool st[N];

// topsort
int d[N], q[N], hh = 0, tt = -1;

// prim
int dist[N], INF = 0x3f3f3f3f;

// kruskal
int p[N];
int egs = 10;
struct Edges {
    int a, b, w;
    bool operator<(const Edges &t) const {
        return w < t.w;
    }
}edges[N];

// 并查集
int find(int x) {
    if (p[x] != x) p[x] = find(p[x]);
    return p[x];
}

// void create() {
//     g[1][2] = 3;
//     d[2] ++;
//     g[1][3] = 3;
//     d[3] ++;
//     g[1][4] = 6;
//     d[4] ++;
//     g[2][3] = 4;
//     d[3] ++;
//     g[2][5] = 5;
//     d[5] ++;
//     g[3][5] = 4;
//     d[5] ++;
//     g[4][6] = 5;
//     d[6] ++;
//     g[5][7] = 3;
//     d[7] ++;
//     g[6][3] = 3;
//     d[3] ++;
//     g[6][7] = 7;
//     d[7] ++;
// }

void create() {
    edges[0] = { 1, 2, 3 };
    edges[1] = { 1, 3, 3 };
    edges[2] = { 1, 4, 6 };
    edges[3] = { 2, 5, 5 };
    edges[4] = { 2, 3, 4 };
    edges[5] = { 3, 5, 4 };
    edges[6] = { 4, 6, 5 };
    edges[7] = { 5, 7, 3 };
    edges[8] = { 6, 3, 3 };
    edges[9] = { 6, 7, 7 };

    std::sort(edges, edges + egs);
}

void bfs(int x = 1) {
    path[++ pp] = x;
    // 攒够7个点 就输出 路径path
    if (pp == nodes - 1) {
        for (int i = 0; i < nodes; i ++) {
            std::cout << path[i] << " ";
        }
        std::cout << "\n";
        return;
    }


    // 循环里暴搜
    for (int i = 1; i <= nodes; i ++) {
        if (g[x][i] != INF && !st[i]) {
            // std::cout << "x = " << x << " i = " << i << "\n";
            // std::cout << "pp = " << tt << " path[pp] = " << path[pp] << "\n";
            st[i] = true;
            bfs(i);
            // 不需要对称，只输出一条路径即可
        }
    }
}

void topsort() {
    for (int i = 1; i <= nodes; i ++)
        if (d[i] == 0) q[++ tt] = i;
    
    while (tt >= hh) {
        int t = q[hh ++];
        // std::cout << "t = " << t << "\n";
        for (int j = 1; j <= nodes; j ++) {
            if (g[t][j] != INF) {
                d[j] --;
                if (d[j] == 0) q[++ tt] = j;
            }
        }
    }
    if (tt != nodes - 1) puts("-1");
    else {
        for (int i = 0; i < nodes; i ++) printf("%d ", q[i]);
    }
}

int prim() {
    int res = 0;

    for (int i = 0; i < nodes; i ++) {
        int t = -1;
        for (int j = 1; j <= nodes; j ++) {
            if (!st[j] && (t == -1 || dist[t] > dist[j]))
                t = j;
        }

        st[t] = true;
        if (i && dist[t] == INF) return INF;
        if (i) res += dist[t];

        for (int k = 1; k <= nodes; k ++) {
            dist[k] = std::min(dist[k], g[t][k]);
        }
    }

    return res;
}

int kruskal() {
    for (int i = 1; i <= egs; i ++) p[i] = i;

    int res = 0, cnt = 0;
    for (int i = 0; i < egs; i ++) {
        int a = edges[i].a, b = edges[i].b, w = edges[i].w;
        a = find(a), b = find(b);
        if (a != b) {
            p[a] = b;
            res += w;
            cnt ++;
        }
    }

    if (cnt < nodes - 1) return -1;
    return res;
}

int main(int argc, char **argv) {
    memset(dist, 0x3f, sizeof dist);
    memset(g, 0x3f, sizeof g);
    create();
    // bfs();
    // topsort();
    // std::cout << prim() << "\n";
    std::cout << kruskal() << "\n";
    return 0;
}
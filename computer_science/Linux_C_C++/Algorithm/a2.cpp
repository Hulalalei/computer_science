#include <iostream>
#include <algorithm>
#include <cstring>

// 邻接表转换为邻接矩阵
constexpr int N = 1e3;
int arc[N][2];  // 0 指向的顶点，1 指向的下一条边
int vertex[N][2]; // 0 顶点信息，1 指向依附此点的边
int idxa = 10, idxv = 10;
int v_num = 3, arc_num = 3;
int path[N], t = -1;
bool v[N];

void AdjGraph();
void readMap();
void dfs(int i, int j);

int main(int argc, char **argv) {
    memset(path, -1, sizeof(path));
    memset(v, 0, sizeof(v));
    memset(arc, -1, sizeof(arc));
    memset(vertex, -1, sizeof(vertex));

    AdjGraph();
    // readMap();
    dfs(1, 2);

    return 0;
}

void AdjGraph() {
    // 存储结点
    for (int i = 0; i < v_num; i ++) vertex[i][0] = i;
    
    // 链接
    int node = 1, prev = -1;
    for (int j = 0; j < v_num; j ++) {
        vertex[j][1] = idxa;
        arc[idxa][0] = idxv;
        vertex[idxv][0] = (node + j) % 3;
        vertex[idxv][1] = ++ idxa;
        arc[idxa][0] = ++ idxv;
        vertex[idxv][0] = (node + j + 1) % 3;
        arc[idxa - 1][1] = idxa;
        arc[idxa][1] = -1;
        idxa ++, idxv ++;
    }
}

void readMap() {
    for (int i = 0; i < v_num; i ++) {
        std::cout << i << "->";
        int p = vertex[i][1];
        while (p != -1) {
            std::cout << vertex[arc[p][0]][0];
            if (arc[p][1] != -1) std::cout << "->";
            p = arc[p][1];
        }
        std::cout << "\n";
    }
}


// 从0到2的所有路径
void dfs(int i, int j) {
    path[++ t] = i;
    v[i] = true;

    for (int k = vertex[i][1]; k != -1 && !v[vertex[arc[k][0]][0]]; k = vertex[arc[k][0]][1]) {
        dfs(vertex[arc[k][0]][0], j);
        v[vertex[arc[k][0]][0]] = false;
        -- t;
    }
    
    if (i == j) {
        for (int i = 0; i <= t; i ++) std::cout << path[i] << " ";
        std::cout << "\n";
    }
}
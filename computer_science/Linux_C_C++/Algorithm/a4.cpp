#include <iostream>
#include <algorithm>
#include <cstring>
#include <queue>


constexpr int N = 1e5 + 10;
int n = 0, m = 0;
int x = 0, y = 0, z = 0;

int h[N], e[N], ne[N], w[N], idx = 0;
int dist[N];
bool st[N];
int q[N], tt = -1, hh = 0;


void add(int x, int y, int z) {
    e[idx] = y, ne[idx] = h[x], w[idx] = z, h[x] = idx ++;
}

int spfa() {
    dist[1] = 0;
    
    q[++ tt] = 1;
    st[1] = true;
    
    while (tt >= hh) {
        int t = q[hh ++];
        st[t] = false;
        
        for (int i = h[t]; i != -1; i = ne[i]) {
            int j = e[i];
            if (dist[j] > dist[t] + w[i]) {
                dist[j] = dist[t] + w[i];
                if (!st[j]) {
                    q[++ tt] = j;
                    st[j] = true;
                }
            }
        }
    }
    
    if (dist[n] > 0x3f3f3f3f / 2) return -1;
    return dist[n];
}

int main(int argc, char **argv) {
    memset(dist, 0x3f, sizeof dist);
    memset(h, -1, sizeof h);
    
    scanf("%d%d", &n, &m);
    
    while (m --) {
        scanf("%d%d%d", &x, &y, &z);
        add(x, y, z);
    }
    
    int t = spfa();
    
    if (t == -1) std::cout << "impossible\n";
    else std::cout << t << "\n";
    
    return 0;
}
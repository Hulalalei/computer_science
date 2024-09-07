#include <iostream>
#include <algorithm>
#include <cstring>
#include <thread>
#include <memory>


constexpr int N = 1e3;
int son[N][2], idx = 0;
bool v[N];
int stk[N], t = -1;

void tree(int node) {
    if (idx > 3) return;

    son[node][0] = ++ idx;
    tree(idx);

    son[node][1] = ++ idx;
    tree(idx);
}

void read(int node) {
    std::cout << node << " ";

    if (son[node][0] == -1) return;
    read(son[node][0]);

    if (son[node][1] == -1) return;
    read(son[node][1]);
}

void round_read() {
    int index = 0, temp = 0, h_max = 1, h_cur = 1;
    stk[++ t] = index;
    while (t != -1) {
        if (son[stk[t]][0] != -1 && !v[son[stk[t]][0]]) {
            h_cur ++;
            v[son[stk[t]][0]] = true;
            temp = son[stk[t]][0];
            stk[++ t] = temp;
            continue;
        }
        if (son[stk[t]][1] != -1 && !v[son[stk[t]][1]]) {
            h_cur ++;
            v[son[stk[t]][1]] = true;
            temp = son[stk[t]][1];
            stk[++ t] = temp;
            continue;
        }

        if (son[stk[t]][0] == -1 && son[stk[t]][1] == -1) {
            h_max = std::max(h_max, h_cur);
            std::cout << stk[t --] << " ";
        }
        if (son[stk[t]][0] != -1 && son[stk[t]][1] != -1) {
            h_cur --;
            if (v[son[stk[t]][0]] && v[son[stk[t]][1]]) std::cout << stk[t --] << " ";
        }
        if (son[stk[t]][0] == -1 && son[stk[t]][1] != -1) {
            h_cur --;
            if (v[son[stk[t]][1]]) std::cout << stk[t --] << " ";
        }
        if (son[stk[t]][0] != -1 && son[stk[t]][1] == -1) {
            h_cur --;
            if (v[son[stk[t]][0]]) std::cout << stk[t --] << " ";
        }
    }
    std::cout << "h_max = " << h_max << std::endl;
}

int main(int argc, char **argv) {
    memset(son, -1, sizeof(son));
    memset(v, 0, sizeof(v));
    memset(stk, 0, sizeof stk);
    tree(0);
    // read(0);
    round_read();

    return 0;
}
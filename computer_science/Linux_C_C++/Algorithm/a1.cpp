#include <iostream>
#include <algorithm>
#include <cstring>


// 先序：012345678
// 中序：4352617 0 8
int A[10] = { 0 }, B[10] = { 0 };
int h1 = 0, t1 = 0, h2 = 0, t2 = 0;
const int M = 1e3;
int son[M][2];

int pre_in_create(int *A, int *B, int h1, int t1, int h2, int t2) {
    int root = A[h1];
    int i = h2;
    for (; B[i] != root; i ++);
    int llen = i - h2;
    int rlen = t2 - i;

    if (llen) son[root][0] = pre_in_create(A, B, h1 + 1, h1 + llen, h2, h2 + llen - 1);
    if (rlen) son[root][1] = pre_in_create(A, B, t1 - rlen + 1, t1, t2 - rlen + 1, t2);
    return root;
}

void read(int node) {
    std::cout << node << " ";

    if (son[node][0] == -1) return;
    read(son[node][0]);

    if (son[node][1] == -1) return;
    read(son[node][1]);
}

int main(int argc, char **argv) {
    memset(son, -1, sizeof(son));
    int A[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
    int B[10] = { 4, 3, 5, 2, 6, 1, 7, 0, 8 };
    pre_in_create(A, B, 0, 8, 0, 8);
    read(0);
    return 0;
}
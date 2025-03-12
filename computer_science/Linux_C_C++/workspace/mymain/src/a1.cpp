#include <cstring>
#include <iostream>
#include <algorithm>


int n = 0, r = 0;
char stk[10];
int hh = -1;

void solve(int n, int r) {
    while (n > 0) {
        int tmp = n % r;
        char c;
        if (tmp > 10) {
            c = tmp - 10 + 65;
            stk[++ hh] = c;
        } else {
            stk[++ hh] = n % r + 48;
        }
        n /= r;
    }
    for (int i = 9; i >= 0; i --) 
        if (stk[i] != -1)
            std::cout << stk[i];
}

int main() {
    memset(stk, -1, sizeof stk);
    while (std::cin >> n >> r) {
        solve(n, r);
    }
}

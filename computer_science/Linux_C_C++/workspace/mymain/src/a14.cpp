#include <iostream>
#include <algorithm>
#include <cstdio>
#include <ostream>


bool isprimer(int a) {
    for (int i = 2; i <= a / i; i ++) {
        if (a % i == 0) return false;
    }
    return true;
}

bool issymmetry(int a) {
    std::string s = {};
    // 数字转字符
    while (a != 0) {
        int tmp = a % 10;
        a /= 10;
        s += (char)(tmp + 48);
    }

    // 判断回文
    int h = 0, t = s.size() - 1;
    while (h <= t) {
        if (s[h] != s[t]) return false;
        h ++, t --;
    }
    return true;
}


int main() {
    for (int i = 2; i < 1e8; i ++) {
        if (isprimer(i) && issymmetry(i))
            std::cout << i << " ";
    }

    return 0;
}

#include <iostream>
using namespace std;


int fun(int n) {
    int ans = 0, tmp = 1;
    while (n != 0) {
        int t = n % 10;
        if (t % 2 == 1) {
            ans += t * tmp;
            tmp *= 10;
        }
        n /= 10;
    }
    return ans;
}

int main() {
    cout << fun(12345);

    return 0;
}

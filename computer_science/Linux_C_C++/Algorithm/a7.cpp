#include <iostream>
#include <algorithm>
#include <cstring>
#include <memory>
#include <exception>


constexpr int N = 128;
int a[N] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

int binary(int begin, int end, int x) {
    int res = -1;
    int mid = (begin + end) / 2;
    std::cout << "mid = " << a[mid] << std::endl;
    if (a[mid] == x) return mid;
    if (begin == end) return res;
    if (a[mid] > x) res = binary(begin, mid - 1, x);
    if (a[mid] < x) res = binary(mid + 1, end, x);
    else return res;
}

int main(int argc, char **argv) {
    try {
        std::cout << binary(0, 9, 11);
    }
    catch (std::exception &e) {
        std::cout << e.what();
    }

    return 0;
}
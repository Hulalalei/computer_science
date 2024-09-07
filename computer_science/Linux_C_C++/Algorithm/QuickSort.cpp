#include <iostream>

const int N = 1e6 + 10;
int n = 0;
int arr[N];

void quick_sort(int l, int r, int *arr) {
    if (l >= r) return;
    
    int i = l - 1, j = r + 1;
    int x = arr[(l + r) / 2];
    
    while (i < j) {
        do i++; while (arr[i] < x);
        do j--; while (arr[j] > x);
        if (i < j) std::swap(arr[i], arr[j]);
    }
	
    //有边界问题，取j即可
    quick_sort(l, j, arr);
    quick_sort(j + 1, r, arr);
}


int main(void) {
    std::cin >> n;
    for (int i = 0; i < n; i++) std::cin >> arr[i];
    
    quick_sort(0, n - 1, arr);
    for (int j = 0; j < n; j++) std::cout << arr[j] << " ";
}
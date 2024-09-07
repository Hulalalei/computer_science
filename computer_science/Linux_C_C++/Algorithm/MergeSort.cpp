#include <iostream>

int n = 0;
const int N = 1e6 + 10;
int arr[N], temp[N];

void merge_sort(int l, int r, int *arr) {
    if (l >= r) return;
    
    int tar = l + r >> 1;
    merge_sort(l, tar, arr);
    merge_sort(tar + 1, r, arr);
    
    int i = l, j = tar + 1, k = 0;
    while (i <= tar && j <= r) {
        if (arr[i] < arr[j]) temp[k++] = arr[i++];
        else temp[k++] = arr[j++];
    }
    while (i <= tar) temp[k++] = arr[i++];
    while (j <= r) temp[k++] = arr[j++];
    
    for (int i = l, k = 0; i <= r; i++, k++) arr[i] = temp[k];
}

int main(void) {
    std::cin >> n;
    for (int i = 0; i < n; i++) std::cin >> arr[i];
    merge_sort(0, n - 1, arr);
    for (int j = 0; j < n; j++) std::cout << arr[j] << " ";
    
    return 0;
}

/*
#include <iostream>

typedef long long LL;

const int N = 1e6 + 10;
int arr[N], temp[N];
int n = 0;
//LL res = 0;

LL merge_sort(int arr[], int l, int r) {
    if (l >= r) return 0;
    
    int mid = l + r >> 1;
    //分治，先获取左右两边的逆序数
    //递归获取逆序数
    LL res = 0;
    res += merge_sort(arr, l, mid) + merge_sort(arr, mid + 1, r);
    
    //获取分散在两边的逆序数
    int k = 0, i = l, j = mid + 1;
    while (i <= mid && j <= r) {
        if (arr[i] <= arr[j]) temp[k++] = arr[i++];
        else {
            res += mid - i + 1;
            temp[k++] = arr[j++];
        }
    }
    
    while (i <= mid) temp[k++] = arr[i++];
    while (j <= r) temp[k++] = arr[j++];
    
    for (int i = l, j = 0; i <= r; i++, j++) arr[i] = temp[j];
    
    return res;
}


int main (void) {
    scanf ("%d", &n);
    for (int i = 0; i < n; i++) scanf ("%d", &arr[i]);
    
    std::cout << merge_sort(arr, 0, n - 1) << std::endl;
}
*/

/*
#include <iostream>

int n = 0;
const int N = 1e6 + 10;
int arr[N] ,temp[N];
typedef long long int LL;

LL merge_sort(int l, int r, int *arr) {
    if (l >= r) return 0;
    
    int res = 0;
    int tar = (l + r) / 1;
    res = merge_sort(l, tar, arr) + merge_sort(tar + 1, r, arr);
    
    int i = l, j = tar + 1, k = 0;
    while (i <= tar && j <= r) {
        if (arr[i] > arr[j]) {
            temp[k++] = arr[j++];
            res += tar - i + 1;
        } else temp[k++] = arr[i++];
    }
    while (i <= tar) temp[k++] = arr[i++];
    while (j <= r) temp[k++] = arr[j++];
    
    for (int i = l, k = 0; i <= r; i++, k++) arr[i] = temp[k];
    return res;
}

int main(void) {
    std::cin >> n;
    for (int i = 0; i < n; i++) std::cin >> arr[i];
    
    std::cout << merge_sort(0, n - 1, arr);
    
    return 0;
}
*/
// #include <iostream>
// #include <algorithm>
//
//
// constexpr int N = 1e2, n = 6, size = n;
// int seq[N] = { 10, 14, 26, 29, 41, 52 };
// int times = 0;
//
// // 顺序插排
// void insert_sort(int *seq, int n) {
//     // 读取2 - n个元素
//     for (int i = 1; i < n; i ++) {
//         // 将2 - n个元素插入到前面
//         for (int j = 0; j < i; j ++) {
//             if (seq[i] < seq[j]) {
//                 int temp = seq[i];
//                 for (int k = i; k > j; k --) {
//                     seq[k] = seq[k - 1];
//                 }
//                 seq[j] = temp;
//             }
//         }
//         // 查看排序过程
//         // for (int i = 0; i < n; i ++)
//         //     std::cout << seq[i] << " ";
//         // std::cout << "\n";
//     }
// }
// // 希尔排序
// void shell_sort(int *seq) {
//     int gap = n / 2;
//     while (gap > 0) {
//
//         // 在子列中进行插排
//         for (int i = 0; i < n; i ++) {
//             int s[N], hh = -1, t = i;
//             while (t < n && gap) {
//                 s[++ hh] = seq[t];
//                 t += gap;
//             }
//             insert_sort(s, hh + 1);
//
//             int tt = -1;
//             t = i;
//             while (t < n && !(tt > hh) && gap) {
//                 seq[t] = s[++ tt];
//                 t += gap;
//             }
//         }
//         gap /= 2;
//     }
// }
// // 冒泡排序
// void bubble_sort(int *seq) {
//     for (int i = 0; i < n; i ++) {
//         for (int j = n - 1; j > i; j --) {
//             if (seq[j - 1] < seq[j]) {
//                 int temp = seq[j];
//                 seq[j] = seq[j - 1];
//                 seq[j - 1] = temp;
//                 times ++;
//             }
//         }
//     }
// }
// // 快速排序
// void quick_sort(int *seq, int start, int end) {
//     if (start >= end) return;
//
//     int l = start - 1;
//     int r = end + 1;
//     int x = seq[l + r >> 1];
//
//     while (l < r) {
//         do l++; while (seq[l] < x);
//         do r--; while (seq[r] > x);
//         if (l < r) std::swap(seq[l], seq[r]);
//     }
//
//     quick_sort(seq, start, r);
//     quick_sort(seq, r + 1, end);
// }
// // 堆排序，小根堆
// void down(int x) {
//     int t = x;
//     if (x * 2 <= size && seq[x * 2] < seq[t]) t = x * 2;
//     if (x * 2 + 1 <= size && seq[x * 2 + 1] < seq[t]) t = x * 2 + 1;
//     if (x != t) {
//         std::swap(seq[x], seq[t]);
//         down(t);
//     }
// }
// void up(int x) {
//     while (x / 2 && seq[x / 2 > seq[x]]) {
//         std::swap(seq[x / 2], seq[x]);
//         x /= 2;
//     }
// }

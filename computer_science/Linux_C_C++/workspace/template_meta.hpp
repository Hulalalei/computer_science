//
// Created by Administrator on 2024/7/5.
//

#ifndef TMP_H
#define TMP_H


template <bool flag, typename T1, typename T2>
class binary;

template <typename T1, typename T2>
class binary<true, T1, T2> {
public:
    typedef T1 ResultT;
};
template <typename T1, typename T2>
class binary<false, T1, T2> {
public:
    typedef T2 ResultT;
};


template <int N, int low = 2, int high = N>
class prime {
public:
    typedef typename binary< (high % low != 0), prime<N, low + 1, high>, prime<N, low, low> >::ResultT flag;
    enum { value = flag::value };
};

template <int N, int low>
class prime<N, low, low> {
public:
    enum { value = (low == N) };
};

#endif //TMP_H

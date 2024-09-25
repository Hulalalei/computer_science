#include <iostream>
#include <format>
#include <string>

// template <class Tb, class ...Ts>
// void print(Tb tb, Ts ...ts) {
//     std::cout << std::format("{}", tb);
//     if constexpr (sizeof...(Ts) != 0) {
//         print(ts...);
//     }
//
// }

template <class ...Ts>
void print(Ts ...ts) {
    ((std::cout << ts), ...);
}

// template <class ...Ts>
// struct common_type {
//     using type = decltype((std::declval<Ts>() + ...));
// };

int main(int argc, char **argv) {
    print(1, 2, 3);
    // using what = typename common_type<int, float, double>::type;

    return 0;
}

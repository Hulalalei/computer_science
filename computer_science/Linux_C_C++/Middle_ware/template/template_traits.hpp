#define CXX 20

#if CXX == 11
// C++ 11
// traits class
struct true_type {};
struct false_type {};

template <class T>
struct traits{
    using is_trivial_type = false_type;
};

template <>
struct traits<int> {
    using is_trivial_type = true_type;
};

struct policy {
    template <class T>
    static void speak(T data, true_type) {
        std::cout << std::format("data is {} and is false\n", data);
    }

    template <class T>
    static void speak(T data, false_type) {
        std::cout << std::format("data is {} and is true\n", data);
    }
};

template <class T, class Policy = policy, class Traits = traits<T>>
void speaks(T data) {
    Policy::speak(data, typename Traits::is_trivial_type{});
} 


#elif CXX == 20
// C++ 20
template <class T>
void speak(T data) {
    if constexpr (requires { std::is_same_v<std::decay_t<T>, int>; })
        std::cout << std::format("data is {} and is true\n", data);
    else 
        std::cout << std::format("data is {} and is false\n", data);
}
#endif

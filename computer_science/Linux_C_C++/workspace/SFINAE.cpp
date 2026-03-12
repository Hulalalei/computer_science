#include <iostream>
#include <unistd.h>

struct value_tag{};
struct pointer_tag{};
struct iterator_tag{};


void override_func(value_tag) { std::cout << "value_tag func\n"; }
void override_func(pointer_tag) { std::cout << "pointer_tag func\n"; }
void override_func(iterator_tag) { std::cout << "iterator_tag func\n"; }


class input_it {
public:
    using value_type = value_tag;
};

template <class T>
class type_traits {
public:
    // 通用接口
    using value_category = typename T::value_type;
};

template <class T>
void callback() {
    override_func(type_traits<input_it>::value_category{});
}


int main() {
    callback<input_it>();
}

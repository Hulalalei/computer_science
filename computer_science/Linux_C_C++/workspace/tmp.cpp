#include <cassert>
#include <iostream>
#include <span>
#include <print>



template <class ...Args>
void bag(Args ...args) {
    std::cout << "hello world\n";
    ((std::cout << "args: " << args), ...);
}

template <class ...Args>
void speak(Args &&...args) {
    bag(std::forward<Args>(args)...);
}

int main() {
    speak(1);
}

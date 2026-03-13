#include <iostream>
#include <math_module.hpp>


int main() {
    std::cout << "this is demo02's main function" << std::endl;
#ifdef CO_ASYNC
    std::cout << sum(15, 25) << std::endl;
#endif
    return 0;
}

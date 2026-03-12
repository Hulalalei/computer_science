#include <iostream>


int main() {
    std::cout << "this is demo01's main function" << std::endl;
    std::cout << "cpp standard_default: " << __cplusplus << std::endl;
#ifdef __GUNC__
    std::cout << "use gnu compiler" << std::endl;
#elif __clang__
    std::cout << "use clang compiler" << std::endl;
#elif _MSC_VER
    std::cout << "use msvc compiler" << std::endl;
#else
    std::cout << "unknown compiler" << std::endl;
#endif
    return 0;
}

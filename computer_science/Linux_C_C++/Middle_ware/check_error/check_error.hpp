#include <source_location>
#include <iostream>
#include <format>


std::string get_filename(std::string pathname) {
    auto pos = pathname.rfind("/");
    return pathname.substr(pos + 1);
}

void check_error(std::string pathname, int line, std::string express, int errno) {
    auto filename = get_filename(pathname);
    std::cout << std::format("{} : {}\t{}, errno: {}", filename, line, express, errno);
}

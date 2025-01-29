#include <cerrno>
#include <netdb.h>
#include <string_view>
#include <sys/socket.h>
#include <string.h>
#include <print>
#include <system_error>

#define CHECK_ERROR(msg, func) do { \
check_error(msg, func);\
} while (false)

int check_error(std::string_view view, int res) {
    if (res == -1) {
        std::println("{}: {}", view, strerror(errno));
        auto ec = std::error_code(errno, std::system_category());
        throw std::system_error(ec, std::string(view));
    }
    return res;
}

int main(int argc, char **argv) {
    struct addrinfo *addrinfo;
    check_error("gai", getaddrinfo("127.0.0.1", "http", nullptr, &addrinfo));

    // int sockfd = check_error("socket", socket(-1, addrinfo->ai_socktype, addrinfo->ai_protocol));
    CHECK_ERROR("socket", socket(addrinfo->ai_family, addrinfo->ai_socktype, addrinfo->ai_protocol));
}

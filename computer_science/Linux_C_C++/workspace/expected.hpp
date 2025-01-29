#pragma once
#include <cstring>
#include <print>
#include <string_view>
#include <system_error>
#include <type_traits>


template <class T>
struct [[nodiscard]] expected {
    std::make_signed_t<T> m_res;

    expected() = default;

    expected(std::make_signed_t<T> res): m_res(res) {}

    bool is_error(int res) {
        // std::println("catch error: {}", strerror(errno));
        return m_res == -res;
    }

    int expect(std::string_view what) {
        // m_res < 0 才能区分出错误, erron > 0
        if (m_res < 0) {
            auto ec = std::error_code(-m_res, std::system_category());
            throw std::system_error(ec, std::string(what));
        }
        return m_res;
    }
};

template <class U, class T>
expected<U> convert_error(T res) {
    if (res == -1) return -errno;
    return res;
}

template <size_t = 0, class T>
expected<T> convert_error(T res) {
    if (res == -1) return -errno;
    return res;
}

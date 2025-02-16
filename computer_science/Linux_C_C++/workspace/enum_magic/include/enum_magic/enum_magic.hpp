#pragma once

#include <cstddef>
#include <string_view>
#include <utility>

// 切割函数原型
// case 1: constexpr std::string_view _enum_type_name() [with CrItMaGiC = 
// code; std::string_view = std::basic_string_view<char>]

// case 2: constexpr std::string_view _enum_value_name() [with auto CrItMa
// GiC = code::GET; std::string_view = std::basic_string_view<char
// >]
constexpr std::string_view _try_extract_value(std::string_view str) {
    auto critpos = str.find("CrItMaGiC = ") + 12;
    // 匹配 ; 与 ] 任意一个字符
    auto endpos = str.find_first_of(";]");
    auto slice = str.substr(critpos, endpos - critpos);
    return slice;
}

// code::GET
constexpr std::string_view _try_remove_prefix(std::string_view str, std::string_view prefix) {
    if (!str.empty()) {
        // 说明无前缀
        if (str.front() == '(') {
            return {};
        }
        if (str.find(prefix) == 0 && str.find("::", prefix.size(), 2) == prefix.size()) {
            return str.substr(prefix.size() + 2);
        }
    }
    return str;
}

// enum class的自定义名称
// f<code>(); -> code
template <class CrItMaGiC>
constexpr std::string_view _enum_type_name() {
    constexpr std::string_view name = _try_extract_value(__PRETTY_FUNCTION__);
    return name;
}

// 获取枚举值的名称
// auto用于推导非类型模板参数
// f<code::GET>(); -> GET
template <auto CrItMaGiC>
constexpr std::string_view _enum_value_name() {
    constexpr auto type = _enum_type_name<decltype(CrItMaGiC)>();
    constexpr std::string_view name = _try_remove_prefix(_try_extract_value(__PRETTY_FUNCTION__), type);
    return name;
}

#if __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wenum-constexpr-conversion"
#endif
// E -> enum class
// 获取E类型枚举的有效范围, sup{enum}
template <class E, size_t I0, size_t I1>
constexpr size_t _enum_range_impl() {
    if constexpr (I0 + 1 == I1) {
        return I1;
    } else {
        constexpr size_t I = (I0 + I1) >> 1;
        // 判断名称是否有效
        if constexpr (!_enum_value_name<static_cast<E>(I)>().empty()) {
            return _enum_range_impl<E, I, I1>();
        } else {
            return _enum_range_impl<E, I0, I>();
        }
    }
}

template <class E>
constexpr size_t _enum_range() {
    return _enum_range_impl<E, 0, 256>();
}
#if __clang__
#pragma clang diagnostic pop
#endif

// enum 转 string
template <class E, size_t ...Is>
constexpr std::string_view _dump_enum_impl(E value, std::index_sequence<Is...>) {
    std::string_view ret;
    (void)((value == static_cast<E>(Is) && ((ret = _enum_value_name<static_cast<E>(Is)>()), false)) || ...);
    return ret;
}

template <class E>
constexpr std::string_view dump_enum(E value) {
    return _dump_enum_impl(value, std::make_index_sequence<_enum_range<E>()>());
}

// string 转 enum
template <class E, size_t ...Is>
constexpr E _parse_enum_impl(std::string_view name, std::index_sequence<Is...>) {
    size_t ret = static_cast<size_t>(-1);
    // 遍历所有Is，将ret设置为满足条件的Is
    (void)((name == _enum_value_name<static_cast<E>(Is)>() && (ret = Is)) || ...);
    return static_cast<E>(ret);
}

template <class E>
constexpr E parse_enum(std::string_view name) {
    // 生成enum的 0 - N-1 个序列
    return _parse_enum_impl<E>(name, std::make_index_sequence<_enum_range<E>()>());
}

#ifndef MINILOG
#define MINILOG

#include <iostream>
#include <fstream>
#include <format>
#include <string>
#include <chrono>
#include <source_location>
#include <stdexcept>

#include <cstring>
#include <cstdint>


// 类内函数是inline，模板是inline，不会重复定义
// static是各文件自己的，inline可以所有文件共享
// 单个<<是原子操作
// usage:
// MINILOG_P(i);
// minilog::set_log_file("./t.log");
// minilog::log_info("hello {}", 42);

namespace minilog {
#define MINILOG_FOREACH_LOG_LEVEL(f) \
    f(trace) \
    f(debug) \
    f(info) \
    f(critical) \
    f(warning) \
    f(error) \
    f(fatal)

enum class log_level : std::uint8_t {
#define _FUNCTION(name) name,
    // _FUNCTION(name) -> name
    MINILOG_FOREACH_LOG_LEVEL(_FUNCTION)
#undef _FUNCTION
};

namespace details {

#if defined(__linux__) || defined(__APPLE__)
inline constexpr char k_level_ansi_colors[(std::uint8_t)log_level::fatal + 1][8] = {
    "\E[37m",
    "\E[35m",
    "\E[32m",
    "\E[34m",
    "\E[33m",
    "\E[31m",
    "\E[31;1m",
};
inline constexpr char k_reset_ansi_color[4] = "\E[m";
#define _MINILOG_IF_HAS_ANSI_COLORS(x) x
#else
#define _MINILOG_IF_HAS_ANSI_COLORS(x)
inline constexpr char k_level_ansi_colors[(std::uint8_t)log_level::fatal + 1][1] = {
    "",
    "",
    "",
    "",
    "",
    "",
    "",
};
inline constexpr char k_reset_ansi_color[1] = "";
#endif

inline std::string log_level_name(log_level lev) {
    switch (lev) {
#define _FUNCTION(name) case log_level::name: return #name;
    MINILOG_FOREACH_LOG_LEVEL(_FUNCTION)
#undef _FUNCTION
    }
    return "unknown";
}

inline log_level log_level_from_name(std::string lev) {
#define _FUNCTION(name) if (lev == #name) return log_level::name;
    MINILOG_FOREACH_LOG_LEVEL(_FUNCTION)
#undef _FUNCTION
    return log_level::info;
}

template <class T>
struct with_source_location {
private:
    T inner;
    std::source_location loc;

public:
    template <class U> requires std::constructible_from<T, U>
    consteval with_source_location(U &&inner, std::source_location loc = std::source_location::current())
    : inner(std::forward<U>(inner)), loc(std::move(loc)) {}
    constexpr T const &format() const { return inner; }
    constexpr std::source_location const &location() const { return loc; }
};

inline log_level g_max_level = [] () -> log_level {
    if (auto lev = std::getenv("MINILOG_LEVEL")) {
        return details::log_level_from_name(lev);
    }
    return log_level::info;
} ();

inline std::ofstream g_log_file = [] () -> std::ofstream {
    if (auto path = std::getenv("MINILOG_FILE")) {
        return std::ofstream(path, std::ios::app);
    }
    return std::ofstream();
} ();

inline void output_log(log_level lev, std::string msg, std::source_location const &loc) {
    std::chrono::zoned_time now{std::chrono::current_zone(), std::chrono::system_clock::now()};

    auto str = std::string(loc.file_name());
    auto pos = str.rfind("/");

    // msg = std::format("{} {}:{} [{}] {}", now, loc.file_name(), loc.line(), details::log_level_name(lev), msg);
    msg = std::format("{} {:20s}:{:4d}\t\t[{}] {}", now, str.substr(pos + 1), loc.line(), details::log_level_name(lev), msg);
    if (g_log_file) {
        // 输出到日志
        g_log_file << msg + '\n';
    }
    if (lev >= g_max_level) {
        // 输出到控制台
        std::cout << _MINILOG_IF_HAS_ANSI_COLORS(k_level_ansi_colors[(std::uint8_t)lev] +)
                    msg _MINILOG_IF_HAS_ANSI_COLORS(+ k_reset_ansi_color) + '\n';
    }
}

}

inline void set_log_file(std::string path) {
    details::g_log_file = std::ofstream(path, std::ios::app);
    if (!details::g_log_file) throw std::runtime_error("can't open or write the log file");
}

inline void set_log_level(log_level lev) {
    details::g_max_level = lev;
}

template <typename... Args>
void generic_log(log_level lev, details::with_source_location<std::format_string<Args...>> fmt, Args &&...args) {
    auto const &loc = fmt.location();
    auto msg = std::vformat(fmt.format().get(), std::make_format_args(args...));
    details::output_log(lev, std::move(msg), loc);
}

#define _FUNCTION(name) \
template <typename... Args> \
void log_##name(details::with_source_location<std::format_string<Args...>> fmt, Args &&...args) { \
    return generic_log(log_level::name, std::move(fmt), std::forward<Args>(args)...); \
}
MINILOG_FOREACH_LOG_LEVEL(_FUNCTION)
#undef _FUNCTION

#define MINILOG_P(x) ::minilog::log_debug(#x "={}", x)

}
#endif
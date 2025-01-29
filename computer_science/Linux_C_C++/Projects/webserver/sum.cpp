module;
#include <future>

export module sum;

import <iostream>;
import <print>;
import minilogm;

export template <class T> int sum(T a, T b) {
    minilog::log_info("hello, {}", "log");
    return a + b;
}

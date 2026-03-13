#include "math_module.hpp"

int sum(int left, int right) {
    return left + right;
}

int divide(int left, int right) {
    return left > right ? left - right : right - left;
}

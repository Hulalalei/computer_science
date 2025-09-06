#include <pybind11/pybind11.h>

int sum(int a, int b) {
    return (a + b);
}

PYBIND11_MODULE(mod01, m) {
    m.def("sum", &sum);
}

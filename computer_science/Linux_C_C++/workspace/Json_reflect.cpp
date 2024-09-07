#include <iostream>
#include <string>
#include <json/json.h>


struct Student {
    std::string name;
    int age;
};

template <class T>
struct reflect_traits {};

template <>
struct reflect_traits<Student> {
    template <class Func>
    static constexpr void for_each_member(Student &stu, Func &&func) {
        func("name", stu.name);
        func("age", stu.age);
    }
};

template <class T>
std::string serialize(T &obj) {
    Json::Value root;
    reflect_traits<T>::for_each_member(obj, [&](const char *key, auto &value) {
        root[key] = value;
    });
    return root.toStyledString();
}

int main(int argc, char **argv) {
    Student stu = {
        .name = "hsf",
        .age = 20,
    };
    auto bin = serialize(stu);
    std::cout << bin << "\n";

    return 0;
}

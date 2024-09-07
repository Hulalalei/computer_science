#include <iostream>
#include <string>


// #if 与 #else的使用，静态多态
#if defined(__GUNC__) || defined(__clang__)
#define NOINLINE __attribute__((noinline))
// [[likely]] and [[unlikely]] 的宏实现，!!表示任何类型都可以转换为bool类型
#define (LIKELY(x)) (__builtin_expect(!!(x), 1))
#define (UNLIKELY(x)) (__builtin_expect(!!(x), 0))
#else 
#define NOINLINE
#define LIKELY(x)
#define UNLIKELY(x)
#endif

// do while防止define展开时，if只识别第一句，后面全部无条件执行了
// \表示换行
#define ERROR(x) do { \
x; \
cout_err(__DATE__, __TIME__, __FILE_NAME__, __LINE__, #x);\
} while(0)



NOINLINE void cout_err(const char *date, const char *time, const char *file_name, int line, const char *expr) {
    std::cout << date << " " << time << " "<< file_name << " line:" << line << " " << expr << std::endl;
}

void speak() {
    std::cout << "hello world!\n";
}

int main(int argc, char **argv) {
    // 先执行x，在调用error函数
    ERROR(speak());
    if (false) [[unlikely]] std::cout << "error\n";
    return 0;
}

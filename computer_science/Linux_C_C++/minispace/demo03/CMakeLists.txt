add_executable(demo03 main.cpp)

# 添加协程支持编译标志
target_compile_options(demo03 PRIVATE -fcoroutines)

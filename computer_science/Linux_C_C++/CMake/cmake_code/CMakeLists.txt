# g++ 支持c++20
# clang 支持c++23
# lsp 采用的是clang的

cmake_minimum_required(VERSION 3.28)
project(main_demo LANGUAGES CXX
                    DESCRIPTION "this is a cmake demo project"
                    VERSION 1.0.0)
set(CMAKE_MESSAGE_LOG_LEVEL "DEBUG")
message(DEBUG "this is a debug message")

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE release)
endif()

# 设置cpp选项
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_COMPILER /usr/bin/clang++)
message(STATUS "C++ compiler ID: ${CMAKE_CXX_COMPILER_ID}")

# 多个main文件的编译选择
# add_subdirectory(demo01)
# add_subdirectory(demo02)
add_subdirectory(module_demo)

# 导入静态库文件
# add_subdirectory(lib)

# 在.cpp文件中设置宏
# 这样，文件中就定义了CO_ASYNC的宏
add_compile_definitions(CO_ASYNC)

# 为目标提供额外的编译选项
target_compile_options(module_demo PRIVATE -std=c++23)

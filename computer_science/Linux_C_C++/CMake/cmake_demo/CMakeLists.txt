cmake_minimum_required(VERSION 3.28)
project(main_demo LANGUAGES CXX
                    DESCRIPTION "this is a cmake demo project"
                    VERSION 1.0.0)
set(CMAKE_MESSAGE_LOG_LEVEL "DEBUG")
message(DEBUG "this is a debug message")

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE release)
endif()

# 多个main文件的编译选择
option(BUILD_demo01 "Build demo01 version" OFF)
option(BUILD_demo02 "Build demo02 version" ON)

if (BUILD_demo01)
    add_subdirectory(demo01)
elseif (BUILD_demo02)
    add_subdirectory(demo02)
endif()

# 导入库文件
add_subdirectory(lib)

# 设置cpp选项
set(CMAKE_CXX_STANARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 在.cpp文件中设置宏
# 这样，文件中就定义了CO_ASYNC的宏
# set(CO_ASYNC ON)
target_compile_definitions(main_demo PRIVATE CO_ASYNC)

# 为目标提供额外的编译选项
# target_compile_options(main_demp PRIVATE -Wall)

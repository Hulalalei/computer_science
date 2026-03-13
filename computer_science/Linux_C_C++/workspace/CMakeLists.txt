cmake_minimum_required(VERSION 3.28)
project(main LANGUAGES CXX
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
# set(CMAKE_CXX_COMPILER /usr/bin/clang++)
message(STATUS "C++ compiler ID: ${CMAKE_CXX_COMPILER_ID}")


# 在.cpp文件中设置宏
# 这样，文件中就定义了CO_ASYNC的宏
# target_compile_definitions(${PROJECT_NAME} PRIVATE CO_ASYNC)
set(CMMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake;${CMAKE_MODULE_PATH}")



add_subdirectory(mymain)
add_subdirectory(co_content)
add_subdirectory(co_async)
add_subdirectory(enum_magic)

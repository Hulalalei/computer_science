cmake_minimum_required(VERSION 3.28)
project(main)

set(CMAKE_CXX_STANDARD 23)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O3 -no-pie -l jemalloc -DJEMALLOC_NO_DEMANGLE")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -no-pie -fmodules-ts")
set(CMAKE_C_COMPILER "/usr/bin/gcc-14")
set(CMAKE_CXX_COMPILER "/usr/bin/g++-14")


include_directories(${PROJECT_SOURCE_DIR})
include_directories(../Middle_ware/lock_free_queue/)
include_directories(../Middle_ware/thread_pool/)
include_directories(../Middle_ware/connection_pool/)
include_directories(../Middle_ware/memory_pool/)
include_directories(../Middle_ware/log/)
include_directories(../Middle_ware/timer/)
include_directories(../Middle_ware/template/)


link_libraries(jsoncpp)
link_libraries(mysqlclient)
link_directories(./)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build)


# 把手动编译的模块目录，拷贝到build目录下即可支持模块功能
add_executable(main main.cpp)
add_executable(tmp tmp.cpp)

target_link_libraries(main PUBLIC mysqlclient)
target_link_libraries(main PRIVATE atomic)

cmake_minimum_required(VERSION 3.28)
project(main)

set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O3 -no-pie -l jemalloc -DJEMALLOC_NO_DEMANGLE")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -no-pie -l uring")


include_directories(${PROJECT_SOURCE_DIR})
include_directories(../Middle_ware/lock_free_queue/)
include_directories(../Middle_ware/thread_pool/)
include_directories(../Middle_ware/connection_pool/)
include_directories(../Middle_ware/memory_pool/)
include_directories(../Middle_ware/log/)
include_directories(../Middle_ware/timer/)
include_directories(../Middle_ware/template/)
include_directories(/usr/include/)

link_libraries(jsoncpp)
link_libraries(mysqlclient)
# link_directories(/usr/local/lib)
link_directories(./)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build)

# add_library(sub SHARED sub.cpp)


add_executable(main coro03.cpp)

target_link_libraries(main PUBLIC mysqlclient)
target_link_libraries(main PRIVATE atomic)

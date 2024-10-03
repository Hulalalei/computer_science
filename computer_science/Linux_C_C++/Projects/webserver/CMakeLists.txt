cmake_minimum_required(VERSION 3.28)
project(main)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O2 -no-pie -l jemalloc -DJEMALLOC_NO_DEMANGLE")

include_directories(${PROJECT_SOURCE_DIR})
include_directories(../../Middle_ware/lock_free_queue/)
include_directories(../../Middle_ware/thread_pool/)
include_directories(../../Middle_ware/connection_pool/)
include_directories(../../Middle_ware/memory_pool/)
include_directories(../../Middle_ware/log/)
include_directories(../../Middle_ware/timer/)
include_directories(../../Middle_ware/http_conn/)
include_directories(../../Middle_ware/co_async/)
include_directories(/usr/include/)

link_libraries(jsoncpp)
link_libraries(mysqlclient)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build)


add_executable(main main.cpp)

target_link_libraries(main PUBLIC mysqlclient)
link_directories(/usr/lib/x86_64-linux-gnu/)
target_link_libraries(main PUBLIC jsoncpp)
target_link_libraries(main PRIVATE atomic)

# https://9026fnej5621.vicp.fun

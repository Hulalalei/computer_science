cmake_minimum_required(VERSION 3.28)
project(main)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -no-pie")

include_directories(${PROJECT_SOURCE_DIR})

link_libraries(jsoncpp)
link_libraries(mysqlclient)
link_directories(/usr/local/lib)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build)

add_executable(main main.cpp)

target_link_libraries(main PUBLIC mysqlclient)
target_link_libraries(main PRIVATE atomic)

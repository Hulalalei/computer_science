cmake_minimum_required(VERSION 3.28)
project(main)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_COMPILER "/usr/bin/g++-14")

add_executable(main t.cpp)

cmake_minimum_required(VERSION 3.28)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_STANDARD 26)

project(co_http LANGUAGES CXX)
set(CMAKE_CXX_COMPILER "/usr/bin/g++-14")

add_compile_options(-Wall -Wextra -Werror=return-type)

include(CheckSymbolExists)
check_symbol_exists(epoll_pwait2 "sys/epoll.h" HAS_epoll_pwait2)
if (HAS_epoll_pwait2)
    add_definitions(-DHAS_epoll_pwait2)
endif()

find_package(Threads REQUIRED)

link_libraries(Threads::Threads)
include_directories(include)

file(GLOB sources CONFIGURE_DEPENDS *.cpp)
foreach (source IN ITEMS ${sources})
    get_filename_component(name "${source}" NAME_WLE)
    add_executable(${name} ${name}.cpp)
endforeach()

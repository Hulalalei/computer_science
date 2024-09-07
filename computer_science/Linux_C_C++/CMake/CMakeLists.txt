# 1、在项目目录中，创建CMakeLists.txt文件
# 2、在此创建一个子目录build，在里面执行cmake ..命令(指定CMakeLists.txt的位置)


cmake_minimum_required(VERSION 3.28)
project(MAIN)

enable_language(ASM)

### set 类似于宏
set(CMAKE_CXX_STANDARD 20)
# set(HOME /home/hsf/computer_science/Linux_C_C++/workspace/)
# set(EXECUTABLE_OUTPUT_PATH ${HOME})


### 查找文件
aux_source_directory(${PROJECT_SOURCE_DIR} src)
# file(GLOB src ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)


### 指定头文件目录
include_directories(${PROJECT_SOURCE_DIR}/include)


### 制作静态库 && 动态库
### 发布库时，要发布: 库 + include；库 = 二进制形式的cpp文件
# set(LIBRARY_OUTPUT_PATH )
add_library(lib_name STATIC t1.cpp t2.cpp)
add_library(lib_name SHARED t1.cpp t2.cpp)


### 链接静态库 libweb.a
# link_libraries(web)
### 指定so & a 库路径，系统库直接链接名字即可，但自己写的，需要指定路径
# link_directories(a_path)


# link_directories(so_path)
add_executable(main ${src})
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -masm=att -no-pie")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -no-pie")


### 链接动态库 libweb.so
# target_link_libraries(main PUBLIC/PRIVATE/INTERFACE web)
# target: 源文件、动态库文件、可执行文件
# PUBLIC 具有传递性；
# PRIVATE 只能传递到target
# INTERFACE 只能传递符号，不知道函数所有者

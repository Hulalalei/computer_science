add_executable(demo02 main.cpp ../lib/math_module.cpp)
# 库 路径
target_link_directories(demo02 PRIVATE math_module)

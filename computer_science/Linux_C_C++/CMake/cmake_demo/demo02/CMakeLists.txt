

add_executable(${PROJECT_NAME} main.cpp ../lib/math_module.cpp)
# 库 路径
target_link_directories(${PROJECT_NAME} PRIVATE math_module)

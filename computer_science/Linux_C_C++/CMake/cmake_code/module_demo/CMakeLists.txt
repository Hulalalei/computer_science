add_executable(module_demo main.cpp)
target_sources(module_demo PUBLIC FILE_SET CXX_MODULES FILES "${CMAKE_CURRENT_SOURCE_DIR}/modules/math.cppm")

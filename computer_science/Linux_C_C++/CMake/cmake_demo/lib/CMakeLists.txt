file(GLOB_RECURSE srcs CONFIGURE_DEPENDS ./*.hpp ./*.cpp)
add_library(math_module STATIC ${srcs})
set_target_properties(math_module PROPERTIES VERSION 1.0.0 DEBUG_POSTFIX "_d")
target_include_directories(${PROJECT_NAME} PRIVATE ./)

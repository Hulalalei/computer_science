file(GLOB pro CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_executable(module_demo ${pro})
# 软编码所有cppm文件
file(GLOB srcs CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/modules/*/*.cppm")
target_sources(module_demo PUBLIC FILE_SET CXX_MODULES FILES ${srcs})

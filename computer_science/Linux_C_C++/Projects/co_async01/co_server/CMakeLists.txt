file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.hpp)
add_library(co_server STATIC ${srcs})
target_include_directories(co_server PUBLIC include)

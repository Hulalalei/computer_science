file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.hpp)
add_library(enum_magic STATIC ${srcs})
target_include_directories(enum_magic PUBLIC include)

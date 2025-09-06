file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.hpp)
add_library(co_task STATIC ${srcs})
target_include_directories(co_task PUBLIC include)

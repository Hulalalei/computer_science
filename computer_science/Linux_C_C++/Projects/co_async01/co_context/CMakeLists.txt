file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.hpp)
add_library(co_context STATIC ${srcs})
target_include_directories(co_context PUBLIC include)

target_link_libraries(co_context PUBLIC utils)

file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.hpp)
add_executable(main ${srcs})
target_include_directories(main PUBLIC include)

find_package(fmt REQUIRED)
target_link_libraries(main PRIVATE fmt::fmt)
target_link_libraries(main PUBLIC utils)
target_link_libraries(main PUBLIC co_context)
target_link_libraries(main PUBLIC co_server)

file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.hpp)
add_executable(mymain ${srcs})
target_include_directories(mymain PUBLIC include)

target_link_libraries(mymain PUBLIC co_task)
target_link_libraries(mymain PRIVATE co_async)
target_link_libraries(mymain PUBLIC enum_magic)

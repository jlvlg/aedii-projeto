add_subdirectory(data)

add_executable(main main.c)
target_link_libraries(main Table ${CURSES_LIBRARIES} "/usr/lib/x86_64-linux-gnu/libpanel.so" "/usr/lib/x86_64-linux-gnu/libmenu.so")
target_include_directories(main PRIVATE ${CURSES_DIRS})

install(TARGETS main)
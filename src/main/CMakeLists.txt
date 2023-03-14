add_subdirectory(data)

add_executable(Main main.c)
target_link_libraries(Main Table ${CURSES_LIBRARIES} "/usr/lib/x86_64-linux-gnu/libpanel.so" "/usr/lib/x86_64-linux-gnu/libmenu.so")
target_include_directories(Main PRIVATE ${CURSES_DIRS})

install(TARGETS Main)
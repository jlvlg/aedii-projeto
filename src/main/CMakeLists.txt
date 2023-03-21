add_subdirectory(data)

add_executable(main main.c)
target_link_libraries(main Table ${CURSES_LIBRARIES} panel menu)
target_include_directories(main PRIVATE ${CURSES_DIRS})

install(TARGETS main)
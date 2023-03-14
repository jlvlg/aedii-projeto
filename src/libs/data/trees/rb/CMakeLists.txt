add_library(RB STATIC rb.c)
target_include_directories(RB
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(RB Tree)

install(TARGETS RB)
install(FILES rb.h DESTINATION include)
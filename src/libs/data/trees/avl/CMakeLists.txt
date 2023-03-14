add_library(AVL STATIC avl.c)
target_include_directories(AVL
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(AVL Tree)

install(TARGETS AVL)
install(FILES avl.h DESTINATION include)
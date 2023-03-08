add_library(AVL avl.c)
target_include_directories(AVL
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(AVL Tree Types Util)
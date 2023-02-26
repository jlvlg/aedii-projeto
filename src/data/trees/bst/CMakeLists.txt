add_library(BST bst.c)
target_include_directories(BST
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(BST Tree Types Util)
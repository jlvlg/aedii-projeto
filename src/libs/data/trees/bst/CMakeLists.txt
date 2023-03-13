add_library(BST STATIC bst.c)
target_include_directories(BST
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(BST Tree)

install(TARGETS BST)
install(FILES bst.h DESTINATION include)
add_library(Table table.c)
target_include_directories(Table
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Table Index Types AVL BST RB json-c)

add_executable(Main main.c)
target_link_libraries(Main Table)
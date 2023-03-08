add_library(Index index.c)
target_include_directories(Index
INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Index RB AVL BST Tree Types Util json-c)
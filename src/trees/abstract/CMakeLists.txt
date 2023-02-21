add_library(Tree tree.c)
target_include_directories(Tree
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Tree Types Util)
add_library(Tree STATIC tree.c)
target_include_directories(Tree
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Tree Types)

install(TARGETS Tree)
install(FILES tree.h DESTINATION include)
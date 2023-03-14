add_library(Index STATIC index.c)
target_include_directories(Index
INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Index RB AVL BST json-c)

install(TARGETS Index)
install(FILES index.h DESTINATION include)
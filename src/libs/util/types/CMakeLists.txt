add_library(Types types.c)
target_include_directories(Types
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Types Util)
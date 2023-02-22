add_library(RB rb.c)
target_include_directories(RB
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(RB Tree Types Util)
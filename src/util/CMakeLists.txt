add_library(Util util.c)
target_include_directories(Util
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(types)
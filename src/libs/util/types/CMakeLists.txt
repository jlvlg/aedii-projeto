add_library(Types STATIC types.c)
target_include_directories(Types
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Types Util)

install(TARGETS Types)
install(FILES types.h DESTINATION include)
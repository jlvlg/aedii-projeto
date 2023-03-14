add_library(Util STATIC util.c)
target_include_directories(Util
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)

install(TARGETS Util)
install(FILES util.h DESTINATION include)

add_subdirectory(types)
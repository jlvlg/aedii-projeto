add_library(Data STATIC data.c)
target_include_directories(Data
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Data Util json-c)

add_library(Table STATIC table.c)
target_include_directories(Table
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Table Index Data)

install(TARGETS Data Table)
install(FILES data.h table.h DESTINATION include)
cmake_minimum_required(VERSION 3.16)
project(AEDII1VA)

set(CMAKE_INSTALL_PREFIX "../bin")
set(BUILD_SHARED_LIBS OFF)

find_package(Curses REQUIRED)

add_subdirectory(src)
include(CTest)
add_subdirectory(tests)
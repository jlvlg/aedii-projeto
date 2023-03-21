cmake_minimum_required(VERSION 3.16)
project(AEDII1VA C)

set(CMAKE_INSTALL_PREFIX "../bin")

find_package(Curses REQUIRED)

add_subdirectory(src)
include(CTest)
add_subdirectory(tests)
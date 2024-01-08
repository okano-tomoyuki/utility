cmake_minimum_required(VERSION 3.13)
project(test_cmake LANGUAGES CXX)

set( CMAKE_CXX_COMPILER /usr/bin/g++ )
add_definitions( -std=c++11 -Wall )

add_subdirectory(src)
add_subdirectory(test)
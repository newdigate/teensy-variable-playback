cmake_minimum_required(VERSION 3.5)
project(teensy_resampling_sdreader C CXX)

set(CMAKE_CXX_STANDARD 11)

set (Boost_USE_STATIC_LIBS OFF)
find_package (Boost REQUIRED COMPONENTS unit_test_framework)
include_directories (${Boost_INCLUDE_DIRS})

add_subdirectory( teensy_cores_x86 )
add_subdirectory( common )
add_subdirectory( test_x86 )
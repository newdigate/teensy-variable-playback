cmake_minimum_required(VERSION 3.5)
project(test_resampling_sdreader C CXX)

#set(CMAKE_CXX_STANDARD 11)

include_directories(../teensy_cores_x86)
include_directories(../common)

link_directories(../teensy_cores_x86/cmake-build-debug)
link_directories(../common/cmake-build-debug)

add_executable(test_resampling_sdreader
        runtests.cpp)

target_link_libraries(test_resampling_sdreader teensy_cores_x86)
target_link_libraries(test_resampling_sdreader resampling_common)
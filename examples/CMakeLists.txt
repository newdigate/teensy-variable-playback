cmake_minimum_required(VERSION 3.5)
project(teensy-variable-playback-examples)

option(OPT_BUILD_FOR_LINUX "Build for linux emulator" ON)
if (OPT_BUILD_FOR_LINUX)
    set(BUILD_FOR_LINUX 1)
    add_definitions(-DBUILD_FOR_LINUX)
endif()

set(CMAKE_CXX_STANDARD 11)
add_subdirectory(array)
add_subdirectory(playarrayresmp)
add_subdirectory(sd_raw)

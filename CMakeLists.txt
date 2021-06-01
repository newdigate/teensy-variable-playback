cmake_minimum_required(VERSION 3.5)
project(teensy-resampling C CXX)
set(CMAKE_CXX_STANDARD 11)

option(OPT_BUILD_FOR_LINUX "Build for linux emulator" ON)
if (OPT_BUILD_FOR_LINUX)
    set(BUILD_FOR_LINUX 1)
endif()

add_subdirectory(src)
add_subdirectory(examples)

if (OPT_BUILD_FOR_LINUX)
    add_subdirectory(test)
endif()

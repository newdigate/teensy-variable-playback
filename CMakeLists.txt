cmake_minimum_required(VERSION 3.5)
project(teensy-resampling C CXX)

if (NOT DEFINED ${OPT_BUILD_FOR_LINUX})
    option(OPT_BUILD_FOR_LINUX "Build for linux emulator" OFF)
    if (OPT_BUILD_FOR_LINUX)
        set(BUILD_FOR_LINUX 1)
    endif()
endif()

add_subdirectory(src)
add_subdirectory(examples)

if (OPT_BUILD_FOR_LINUX)
    add_subdirectory(test)
endif()

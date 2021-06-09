cmake_minimum_required(VERSION 3.5)

if (NOT DEFINED ${OPT_BUILD_FOR_LINUX})  
    option(OPT_BUILD_FOR_LINUX "Build for linux emulator" OFF)
    if (OPT_BUILD_FOR_LINUX)
        set(BUILD_FOR_LINUX 1)
        add_definitions(-DBUILD_FOR_LINUX)
    endif()
endif()

add_subdirectory(array)
add_subdirectory(playarrayresmp)
add_subdirectory(sd_raw)

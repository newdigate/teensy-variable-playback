if (NOT DEFINED OPT_BUILD_FOR_LINUX_IS_DEFINED)
    option(OPT_BUILD_FOR_LINUX "Build for linux emulator" OFF)
    set(OPT_BUILD_FOR_LINUX_IS_DEFINED 1)
    if (OPT_BUILD_FOR_LINUX)
        message(STATUS "!!! Build for linux emulator" )
        set(BUILD_FOR_LINUX 1)
        add_definitions(-DBUILD_FOR_LINUX)
    else()
        message(STATUS "!!! Build for teensy" )
        include(../../teensy.options.cmake.in)
        include(../../teensy.build.cmake.in)
    endif()
endif()

cmake_minimum_required(VERSION 3.10)
project(basic)
set(CMAKE_CXX_STANDARD 14)

if (DEFINED BUILD_FOR_LINUX)
    find_package(teensy_x86_stubs)
    include_directories(${teensy_x86_stubs_INCLUDE_DIR})

    find_package(teensy_audio_x86_stubs)
    include_directories(${teensy_audio_x86_stubs_INCLUDE_DIR})
    
    find_package(teensy_x86_sd_stubs)
    include_directories(${teensy_x86_sd_stubs_INCLUDE_DIR})

    add_executable(basic
            sd_raw.cpp
            ../../src/ResamplingArrayReader.cpp
            ../../src/ResamplingSdReader.cpp
            ../../src/playsdwavresmp.cpp
            ../../src/playsdrawresmp.cpp
            ../../src/interpolation.cpp)

    target_link_libraries(basic ${teensy_x86_stubs_LIBS})
    target_link_libraries(basic ${teensy_audio_x86_stubs_LIBS})
    target_link_libraries(basic ${teensy_x86_sd_stubs_LIBS})
    #set(CMAKE_VERBOSE_MAKEFILE  1)
else()
    include(../examples.cmake.in)
    teensy_add_executable(basic sd_raw.cpp)
endif()


#set(CMAKE_VERBOSE_MAKEFILE 1)
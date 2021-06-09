if (NOT DEFINED OPT_BUILD_FOR_LINUX_IS_DEFINED)
    option(OPT_BUILD_FOR_LINUX "Build for linux emulator" OFF)
    set(OPT_BUILD_FOR_LINUX_IS_DEFINED 1)
    if (OPT_BUILD_FOR_LINUX)
        message(STATUS "!!! Build for linux emulator" )
        set(BUILD_FOR_LINUX 1)
        add_definitions(-DBUILD_FOR_LINUX)
    else()
        message(STATUS "!!! Build for teensy" )
        include(teensy.options.cmake.in)
        include(teensy.build.cmake.in)
    endif()
endif()


set(SOURCE_FILES
        playsdrawresmp.cpp
        playsdwavresmp.cpp
        ResamplingSdReader.cpp
        ResamplingArrayReader.cpp
        playarrayresmp.cpp
        interpolation.cpp)

set(HEADER_FILES
        loop_type.h
        playsdrawresmp.h
        playsdwavresmp.h
        ResamplingSdReader.h
        waveheaderparser.h
        ResamplingArrayReader.h
        playarrayresmp.h
        interpolation.h
)

cmake_minimum_required(VERSION 3.5)
project(teensy_variable_playback C CXX)
set(teensy_variable_playback_VERSION 1.0.0)

set(CMAKE_VERBOSE_MAKEFILE 1)
if (DEFINED BUILD_FOR_LINUX)
    include(CMakeLists.linux.cmake.in)
else()
    add_custom_target(installarduino
            "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/install_arduino_library.cmake")
endif()
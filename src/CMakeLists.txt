cmake_minimum_required(VERSION 3.5)
project(teensy_variable_playback C CXX)
set(teensy_variable_playback_VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 11)

option(OPT_BUILD_FOR_LINUX "Build for linux emulator" ON)
if (OPT_BUILD_FOR_LINUX)
    set(BUILD_FOR_LINUX 1)
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

if (DEFINED BUILD_FOR_LINUX)
    include(CMakeLists.linux.cmake.in)
else()
    include(CMakeLists.teensy.cmake.in)
    add_custom_target(installarduino
            "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/install_arduino_library.cmake")
endif()
cmake_minimum_required(VERSION 3.5)
project(teensy_variable_playback C CXX)
set(teensy_variable_playback_VERSION 1.0.0)
set(CMAKE_CXX_STANDARD 11)
set(SOURCE_FILES
        playsdresmp.cpp
        ResamplingSdReader.cpp
        ResamplingArrayReader.cpp
        playarrayresmp.cpp
        interpolation.cpp
        IndexableFile.cpp)

set(HEADER_FILES
        loop_type.h
        playresmp.h
        playsdresmp.h
        ResamplingSdReader.h
        waveheaderparser.h
        ResamplingArrayReader.h
        playarrayresmp.h
        interpolation.h
        TeensyVariablePlayback.h
        IndexableFile.h)

#set(CMAKE_VERBOSE_MAKEFILE 1)
if (NOT DEFINED TEENSY_VERSION)
    include(CMakeLists.linux.cmake.in)
else()
    teensy_add_library(
            teensy_variable_playback
            ${SOURCE_FILES}
    )

    add_custom_target(installarduino
            "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/install_arduino_library.cmake")
endif()
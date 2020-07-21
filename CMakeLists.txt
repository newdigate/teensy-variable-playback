cmake_minimum_required(VERSION 3.5)
project(teensy-resampling-src C CXX)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(teensy)

add_library(teensy-resampling-src STATIC
        ResamplingSdReader.h
        ResamplingSdReader.cpp
        waveheaderparser.cpp
        waveheaderparser.h)

add_subdirectory(audio)
add_subdirectory(test)
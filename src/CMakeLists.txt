cmake_minimum_required(VERSION 3.5)
project(teensy-resampling-src C CXX)

set(CMAKE_CXX_STANDARD 11)

find_package(teensy_x86_stubs)
include_directories(${teensy_x86_stubs_INCLUDE_DIR})
message(STATUS "include-dir: teensy_x86_stubs: ${teensy_x86_stubs_INCLUDE_DIR}")

find_package(midi_smf_reader)
include_directories(${midi_smf_reader_INCLUDE_DIR})
message(STATUS "include-dir: midi_smf_reader: ${midi_smf_reader_INCLUDE_DIR}")

add_library(teensy-resampling-src STATIC
        ResamplingSdReader.h
        ResamplingSdReader.cpp
        waveheaderparser.cpp
        waveheaderparser.h)
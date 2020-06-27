cmake_minimum_required(VERSION 3.5)

project(teensy_midi_common C CXX)

set(CMAKE_CXX_STANDARD 11)

# location of header files
include_directories(../teensy_cores_x86)

add_library(resampling_common STATIC
		ResamplingSdReader.h
		ResamplingSdReader.cpp
        spi_interrupt.h
        spi_interrupt.cpp)
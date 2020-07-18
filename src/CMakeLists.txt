cmake_minimum_required(VERSION 3.5)

project(teensy-resampling-src C CXX)

set(CMAKE_CXX_STANDARD 11)

# location of header files
include_directories(../teensy)

add_library(teensy-resampling-src STATIC
		ResamplingSdReader.h
		ResamplingSdReader.cpp
        spi_interrupt.h
        spi_interrupt.cpp)
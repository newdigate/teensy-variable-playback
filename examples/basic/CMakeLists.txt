cmake_minimum_required(VERSION 3.10)
project(basic)

set(CMAKE_CXX_STANDARD 11)
include(../CMakeLists.cmake.in)

get_filename_component(CURRENT_FOLDER_NAME ${CMAKE_SOURCE_DIR} NAME)
if(CURRENT_FOLDER_NAME STREQUAL "examples")
  import_arduino_library(${CMAKE_SOURCE_DIR} ../src)
else()
    import_arduino_library(${CMAKE_SOURCE_DIR} ../../src)
endif()

teensy_add_executable(basic basic.cpp)
#set(CMAKE_VERBOSE_MAKEFILE 1)
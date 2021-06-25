cmake_minimum_required(VERSION 3.10)
project(array)
set(CMAKE_CXX_STANDARD 14)

find_package(teensy_x86_stubs)
include_directories(${teensy_x86_stubs_INCLUDE_DIR})

find_package(teensy_audio_x86_stubs)
include_directories(${teensy_audio_x86_stubs_INCLUDE_DIR})

find_package(teensy_x86_sd_stubs)
include_directories(${teensy_x86_sd_stubs_INCLUDE_DIR})

include_directories(../../../src)

add_executable(array array.cpp)

target_link_libraries(array ${teensy_x86_stubs_LIBS})
target_link_libraries(array ${teensy_audio_x86_stubs_LIBS})
target_link_libraries(array ${teensy_x86_sd_stubs_LIBS})
target_link_libraries(array teensy_variable_playback)

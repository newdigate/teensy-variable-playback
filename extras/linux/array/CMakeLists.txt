cmake_minimum_required(VERSION 3.10)
project(array)
set(CMAKE_CXX_STANDARD 14)

include_directories(../../../src)

add_executable(array array.cpp)

target_link_libraries(array teensy_x86_stubs)
target_link_libraries(array teensy_audio_x86_stubs)
target_link_libraries(array teensy_x86_sd_stubs)
target_link_libraries(array teensy_variable_playback)
cmake_minimum_required(VERSION 3.10)
project(sd_raw)
set(CMAKE_CXX_STANDARD 14)

include_directories(../../../src)
add_executable(sd_raw sd_raw.cpp)

target_link_libraries(sd_raw teensy_x86_stubs)
target_link_libraries(sd_raw teensy_audio_x86_stubs)
target_link_libraries(sd_raw teensy_x86_sd_stubs)
target_link_libraries(sd_raw teensy_variable_playback)

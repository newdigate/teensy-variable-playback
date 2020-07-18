cmake_minimum_required(VERSION 3.5)
project(teensy_resampling_sdreader C CXX)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(teensy)
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(audio)

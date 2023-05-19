cmake_minimum_required(VERSION 3.5)
project(tests_lowlevel_wavheader C CXX)
set(CMAKE_CXX_STANDARD 14)

set (Boost_USE_STATIC_LIBS OFF)
find_package (Boost REQUIRED COMPONENTS unit_test_framework)
include_directories (${Boost_INCLUDE_DIRS})

find_package(teensy_x86_stubs)
include_directories(${teensy_x86_stubs_INCLUDE_DIR})
message(STATUS "include-dir: teensy_x86_stubs: ${teensy_x86_stubs_INCLUDE_DIR}")

find_package(teensy_audio_x86_stubs)
include_directories(${teensy_audio_x86_stubs_INCLUDE_DIR})

find_package(teensy_x86_sd_stubs)
include_directories(${teensy_x86_sd_stubs_INCLUDE_DIR})

include_directories(../../../src)

add_executable(tests_lowlevel_wavheader
        test_parse_wave_header.cpp
        WaveHeaderParserFixture.h 
        )

target_link_libraries(tests_lowlevel_wavheader ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
target_link_libraries(tests_lowlevel_wavheader ${teensy_x86_stubs_LIBS})
target_link_libraries(tests_lowlevel_wavheader ${teensy_audio_x86_stubs_LIBS})
target_link_libraries(tests_lowlevel_wavheader ${teensy_x86_sd_stubs_LIBS})
message(STATUS ${teensy_x86_stubs_LIBS})
#set(CMAKE_VERBOSE_MAKEFILE 1)
cmake_minimum_required(VERSION 3.5)
project(tests_lowlevel_sdreader C CXX)
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

add_executable(tests_lowlevel_sdreader
        ../../../src/ResamplingSdReader.cpp
        ../../../src/ResamplingArrayReader.cpp
        ../../../src/interpolation.cpp
        test_raw_mono_noloop_forward_playback.cpp
        ResamplingReaderFixture.h
        test_raw_mono_noloop_forward_double_rate_playback.cpp
        test_raw_mono_loop_forward_playback.cpp
        test_wav_mono_noloop_forward_playback.cpp
        test_wav_mono_loop_forward_playback.cpp
        test_wav_mono_noloop_backward_playback.cpp
        #test_parse_wave_header.cpp
        #WaveHeaderParserFixture.h 
        ../../../src/loop_type.h
        #test_array_mono_loop_forward_playback.cpp
        )

target_link_libraries(tests_lowlevel_sdreader ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
target_link_libraries(tests_lowlevel_sdreader ${teensy_x86_stubs_LIBS})
target_link_libraries(tests_lowlevel_sdreader ${teensy_audio_x86_stubs_LIBS})
target_link_libraries(tests_lowlevel_sdreader ${teensy_x86_sd_stubs_LIBS})
message(STATUS ${teensy_x86_stubs_LIBS})
#set(CMAKE_VERBOSE_MAKEFILE 1)
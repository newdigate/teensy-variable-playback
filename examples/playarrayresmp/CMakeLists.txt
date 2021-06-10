cmake_minimum_required(VERSION 3.10)
project(playarrayresmp)
set(CMAKE_CXX_STANDARD 14)

if (DEFINED BUILD_FOR_LINUX)
    find_package(teensy_x86_stubs)
    include_directories(${teensy_x86_stubs_INCLUDE_DIR})

    find_package(teensy_audio_x86_stubs)
    include_directories(${teensy_audio_x86_stubs_INCLUDE_DIR})

    find_package(teensy_x86_sd_stubs)
    include_directories(${teensy_x86_sd_stubs_INCLUDE_DIR})

    add_executable(playarrayresmp
            playarrayresmp.cpp
            ../../src/ResamplingArrayReader.cpp
            ../../src/ResamplingSdReader.cpp
            ../../src/playsdwavresmp.cpp
            ../../src/playsdrawresmp.cpp
            ../../src/playarrayresmp.cpp
            ../../src/interpolation.cpp)

    target_link_libraries(playarrayresmp ${teensy_x86_stubs_LIBS})
    target_link_libraries(playarrayresmp ${teensy_audio_x86_stubs_LIBS})
    target_link_libraries(playarrayresmp ${teensy_x86_sd_stubs_LIBS})
else()
    include(../examples.cmake.in)
    teensy_add_executable(playarrayresmp playarrayresmp.cpp)
endif()


#set(CMAKE_VERBOSE_MAKEFILE 1)
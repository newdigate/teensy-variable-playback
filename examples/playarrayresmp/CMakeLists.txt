cmake_minimum_required(VERSION 3.10)
project(playarrayresmp)

set(CMAKE_CXX_STANDARD 11)

option(OPT_BUILD_FOR_LINUX "Build for linux emulator" ON)
if (OPT_BUILD_FOR_LINUX)
    set(BUILD_FOR_LINUX 1)
    add_definitions(-DBUILD_FOR_LINUX)
endif()

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
    include(../../teensy.options.cmake.in)
    include(../../teensy.build.cmake.in)
    include(../examples.cmake.in)
    include(../CMakeLists.cmake.in)

    teensy_add_executable(playarrayresmp playarrayresmp.cpp)

endif()


#set(CMAKE_VERBOSE_MAKEFILE 1)
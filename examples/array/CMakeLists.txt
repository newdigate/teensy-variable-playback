cmake_minimum_required(VERSION 3.10)
if (NOT DEFINED ${OPT_BUILD_FOR_LINUX})  
    option(OPT_BUILD_FOR_LINUX "Build for linux emulator" OFF)
    if (OPT_BUILD_FOR_LINUX)
        set(BUILD_FOR_LINUX 1)
        add_definitions(-DBUILD_FOR_LINUX)
    endif()
endif()

if (NOT DEFINED BUILD_FOR_LINUX)  
    include(../../teensy.options.cmake.in)
    include(../../teensy.build.cmake.in)
    include(../examples.cmake.in)
endif()

project(array)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_VERBOSE_MAKEFILE 1)

if (DEFINED BUILD_FOR_LINUX)
    find_package(teensy_x86_stubs)
    include_directories(${teensy_x86_stubs_INCLUDE_DIR})

    find_package(teensy_audio_x86_stubs)
    include_directories(${teensy_audio_x86_stubs_INCLUDE_DIR})

    find_package(teensy_x86_sd_stubs)
    include_directories(${teensy_x86_sd_stubs_INCLUDE_DIR})

    add_executable(array
            array.cpp
            ../../src/ResamplingArrayReader.cpp
            ../../src/ResamplingSdReader.cpp
            ../../src/playsdwavresmp.cpp
            ../../src/playsdrawresmp.cpp
            ../../src/interpolation.cpp)

    target_link_libraries(array ${teensy_x86_stubs_LIBS})
    target_link_libraries(array ${teensy_audio_x86_stubs_LIBS})
    target_link_libraries(array ${teensy_x86_sd_stubs_LIBS})
    set(CMAKE_VERBOSE_MAKEFILE  1)
else()
    teensy_add_executable(array array.cpp)

endif()


#set(CMAKE_VERBOSE_MAKEFILE 1)
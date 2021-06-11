cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 11)
project(save_raw)

find_package(teensy_x86_stubs)
include_directories(${teensy_x86_stubs_INCLUDE_DIR})

find_package(teensy_x86_sd_stubs)
include_directories(${teensy_x86_sd_stubs_INCLUDE_DIR})

find_package(teensy_audio_x86_stubs)
include_directories(${teensy_audio_x86_stubs_INCLUDE_DIR})

find_package(teensy_audio_x86_stubs_soundio)
include_directories(${teensy_audio_x86_stubs_soundio_INCLUDE_DIR})

find_package(teensy_st7735_linux_stubs)
include_directories(${teensy_st7735_linux_stubs_INCLUDE_DIR})

find_package(teensy_st7735_linux_opengl)
include_directories(${teensy_st7735_linux_extras_opengl_INCLUDE_DIR})

add_executable(save_raw
        save_raw.cpp
        ../../../src/playarrayresmp.cpp
        ../../../src/interpolation.cpp
        ../../../src/ResamplingArrayReader.cpp
        
    )


#soundio
include_directories(/usr/local/include)

if(WIN32)
    set(LIBS glfw3 opengl32)
elseif(UNIX AND NOT APPLE)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
    find_package(OpenGL REQUIRED)
    add_definitions(${OPENGL_DEFINITIONS})
    find_package(X11 REQUIRED)
    # note that the order is important for setting the libs
    # use pkg-config --libs $(pkg-config --print-requires --print-requires-private glfw3) in a terminal to confirm
    set(LIBS ${GLFW3_LIBRARY} X11 Xrandr Xi Xxf86vm GL dl pthread freetype GLEW glfw)
    set (CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -ldl")
elseif(APPLE)
    INCLUDE_DIRECTORIES(/System/Library/Frameworks)
    FIND_LIBRARY(glfw3_LIBRARY glfw)
    FIND_LIBRARY(COCOA_LIBRARY Cocoa)
    FIND_LIBRARY(OpenGL_LIBRARY OpenGL)
    FIND_LIBRARY(IOKit_LIBRARY IOKit)
    FIND_LIBRARY(glew_LIBRARY glew)
    FIND_LIBRARY(CoreVideo_LIBRARY CoreVideo)
    MARK_AS_ADVANCED(COCOA_LIBRARY OpenGL_LIBRARY)
    FIND_LIBRARY(FREETYPE_LIBRARIES FreeType)
    SET(APPLE_LIBS ${COCOA_LIBRARY} ${IOKit_LIBRARY} ${OpenGL_LIBRARY} ${CoreVideo_LIBRARY})
    SET(APPLE_LIBS ${APPLE_LIBS} ${GLFW3_LIBRARY} ${ASSIMP_LIBRARY} ${FREETYPE_LIBRARIES} ${glfw3_LIBRARY} ${glew_LIBRARY})
    set(LIBS ${LIBS} ${APPLE_LIBS})
endif()

target_link_libraries(save_raw ${LIBS} ${teensy_x86_stubs_LIBS} ${teensy_x86_sd_stubs_LIBS} ${teensy_audio_x86_stubs_LIBS} ${teensy_audio_x86_stubs_soundio_LIBS} ${teensy_st7735_linux_stubs_LIBS} ${teensy_st7735_linux_extras_opengl_LIBS})
target_link_libraries(save_raw /usr/local/lib/libsoundio.a)
target_link_libraries(save_raw
        "-framework CoreServices"
        "-framework CoreAudio"
        "-framework Foundation"
        "-framework AudioUnit")
message(STATUS ${LIBS})
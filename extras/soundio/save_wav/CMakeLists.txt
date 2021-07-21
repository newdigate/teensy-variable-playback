cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 11)
project(save_wav)

find_package(teensy_x86_stubs)
include_directories(${teensy_x86_stubs_INCLUDE_DIR})

find_package(teensy_x86_sd_stubs)
include_directories(${teensy_x86_sd_stubs_INCLUDE_DIR})

find_package(teensy_audio_x86_stubs)
include_directories(${teensy_audio_x86_stubs_INCLUDE_DIR})

find_package(teensy_audio_x86_stubs_soundio)
if(teensy_audio_x86_stubs_soundio_FOUND) 
    include_directories(${teensy_audio_x86_stubs_soundio_INCLUDE_DIR})

    include_directories(/usr/local/include) #soundio
    include_directories(../../../src)

    add_custom_command(OUTPUT stereo_souljah_wav.c
      COMMAND embedfile stereo_souljah_wav ${CMAKE_SOURCE_DIR}/test/resources/input/stereo_souljah.wav
    )

    add_custom_command(OUTPUT mono_souljah_wav.c
      COMMAND embedfile mono_souljah_wav ${CMAKE_SOURCE_DIR}/test/resources/input/mono_souljah.wav
    )

    add_executable(save_wav  mono_souljah_wav.c stereo_souljah_wav.c save_wav.cpp)
    target_link_libraries(save_wav teensy_variable_playback)

    if(WIN32)
    elseif(UNIX AND NOT APPLE)
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
        SET(SOUNDIO_LIBS -L/usr/lib/x86_64-linux-gnu -lsoundio)
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
        target_link_libraries(save_wav
            "-framework CoreServices"
            "-framework CoreAudio"
            "-framework Foundation"
            "-framework AudioUnit")
        SET(SOUNDIO_LIBS /usr/local/lib/libsoundio.a)
    endif()

    target_link_libraries(save_wav ${LIBS} ${teensy_x86_stubs_LIBS} ${teensy_x86_sd_stubs_LIBS} ${teensy_audio_x86_stubs_LIBS} ${teensy_audio_x86_stubs_soundio_LIBS} ${teensy_st7735_linux_stubs_LIBS} ${teensy_st7735_linux_extras_opengl_LIBS} ${SOUNDIO_LIBS})
#set(CMAKE_VERBOSE_MAKEFILE 1)
endif()
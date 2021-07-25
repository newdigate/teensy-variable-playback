cmake_minimum_required(VERSION 3.5)
project(tests C CXX)
if (DEFINED BUILD_FOR_LINUX)
    set(CMAKE_CXX_STANDARD 17)

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

    include_directories(../src)
    include_directories(../test/audio)

    add_executable(embedfile embedfile.c)

    add_custom_command(OUTPUT kick_raw.c
      COMMAND embedfile kick_raw ${CMAKE_SOURCE_DIR}/test/resources/input/kick.raw
    )

    add_custom_command(OUTPUT stereo_souljah_raw.c
      COMMAND embedfile stereo_souljah_raw ${CMAKE_SOURCE_DIR}/test/resources/input/stereo_souljah.raw
    )

    add_custom_command(OUTPUT stereo_souljah_wav.c
      COMMAND embedfile stereo_souljah_wav ${CMAKE_SOURCE_DIR}/test/resources/input/stereo_souljah.wav
    )

    add_custom_command(OUTPUT mono_souljah_wav.c
      COMMAND embedfile mono_souljah_wav ${CMAKE_SOURCE_DIR}/test/resources/input/mono_souljah.wav
    )

    add_custom_command(OUTPUT PNO1C1_raw.c
      COMMAND embedfile PNO1C1_raw ${CMAKE_SOURCE_DIR}/test/resources/input/PNO1C1.raw
    )

    add_executable(tests
            kick_raw.c
            stereo_souljah_raw.c
            stereo_souljah_wav.c
            mono_souljah_wav.c
            PNO1C1_raw.c
            audio/output_test.cpp

            audio/array/AudioArrayFixture.h
            audio/array/test_array_mono_loop_forward_playback.cpp
            audio/array/test_array_stereo_loop_forward_playback.cpp

            audio/wav/AudioWavFixture.h
            audio/wav/test_wav_mono_loop_forward_playback.cpp
            audio/wav/test_wav_stereo_loop_forward_playback.cpp

            low_level/sd/ResamplingReaderFixture.h
            low_level/sd/test_raw_mono_noloop_forward_playback.cpp
            low_level/sd/test_raw_mono_noloop_forward_double_rate_playback.cpp
            low_level/sd/test_raw_mono_loop_forward_playback.cpp
            low_level/sd/test_wav_mono_noloop_forward_playback.cpp
            low_level/sd/test_wav_mono_loop_forward_playback.cpp
            low_level/sd/test_wav_mono_noloop_backward_playback.cpp
            
            low_level/wav_header/test_parse_wave_header.cpp
            
            low_level/array/test_array_mono_loop_forward_playback.cpp
            low_level/array/test_array_stereo_loop_forward_playback.cpp
            
            low_level/arraywav/test_array_mono_loop_forward_playback.cpp
            low_level/arraywav/test_array_stereo_loop_forward_playback.cpp

            low_level/indexedfile/test_indexablefile.cpp
            )

    target_link_libraries(tests ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
    target_link_libraries(tests ${teensy_x86_stubs_LIBS})
    target_link_libraries(tests ${teensy_audio_x86_stubs_LIBS})
    target_link_libraries(tests ${teensy_x86_sd_stubs_LIBS})
    target_link_libraries(tests teensy_variable_playback)
    message(STATUS ${teensy_x86_stubs_LIBS})
endif()
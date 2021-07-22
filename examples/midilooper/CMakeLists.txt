cmake_minimum_required(VERSION 3.10)
project(sampleloader)
set(CMAKE_CXX_STANDARD 14)
teensy_include_directories(../../src)

import_arduino_library(sampleflashloader ${DEPSPATH}/teensy-sample-flashloader/src)
import_arduino_library(midi ${DEPSPATH}/MIDI/src)

teensy_add_executable(sampleloader sampleloader.ino)
teensy_target_link_libraries(sampleloader midi sampleflashloader SD SdFat Audio teensy_variable_playback cores arm_math SPI Wire SerialFlash  )

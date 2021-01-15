cmake_minimum_required(VERSION 3.10)
project(untitled3)

set(CMAKE_CXX_STANDARD 11)

# This toolchain file is based on https://github.com/apmorton/teensy-template/blob/master/Makefile
# and on the Teensy Makefile.

include(CMakeForceCompiler)

# User configurable variables
# (show up in cmake-gui)
#
set(TEENSY_VERSION 40 CACHE STRING "Set to the Teensy version corresponding to your board (30 or 31 allowed)" FORCE)
set(CPU_CORE_SPEED 600000000 CACHE STRING "Set to 24000000, 48000000, 72000000 or 96000000 to set CPU core speed" FORCE) # Derived variables

set(CPU cortex-m7)

set(COMPILERPATH "/opt/gcc-arm-none-eabi-9-2019-q4-major/bin/")
set(DEPSPATH "/home/runner/work/teensy-variable-playback/teensy-variable-playback/deps")
set(COREPATH "${DEPSPATH}/teensy-cores/teensy4/")

# Normal toolchain configuration
#

# this one is important
set(CMAKE_SYSTEM_NAME Generic)

# specify the cross compiler
cmake_force_c_compiler(${COMPILERPATH}arm-none-eabi-gcc GNU)
cmake_force_cxx_compiler(${COMPILERPATH}arm-none-eabi-g++ GNU)

# where is the target environment
set(CMAKE_FIND_ROOT_PATH ${COMPILERPATH})

# search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)


# Additional C and CPP files for Arduino support
#

file(GLOB_RECURSE TEENSY_C_FILES ${COREPATH}*.c)
file(GLOB_RECURSE TEENSY_CPP_FILES ${COREPATH}*.cpp)
set(TEENSY_SOURCES ${TEENSY_C_FILES} ${TEENSY_CPP_FILES})
list(REMOVE_ITEM TEENSY_SOURCES ${COREPATH}main.cpp)

# Teensy 3.0 and 3.1 differentiation
#
message("TEENSY_VERSION ${TEENSY_VERSION}")
if (TEENSY_VERSION EQUAL 30)
    set(CPU_DEFINE __MK20DX128__)
    set(LINKER_FILE ${COREPATH}mk20dx128.ld)
endif()

if (TEENSY_VERSION EQUAL 40)
    set(CPU_DEFINE __IMXRT1062__)
    set(LINKER_FILE ${COREPATH}imxrt1062.ld)
endif()

if(TEENSY_VERSION EQUAL 31)
    set(CPU_DEFINE __MK20DX256__)
    set(LINKER_FILE ${COREPATH}mk20dx256.ld)
endif()

set(TEENSY_DEFINITIONS USB_SERIAL
        LAYOUT_US_ENGLISH
        F_CPU=${CPU_CORE_SPEED}
        ${CPU_DEFINE}
        TEENSYDUINO=151
        ARDUINO=10812
        ARDUINO_TEENSY40
        USB_SERIAL)

message("TEENSY_DEFINITIONS " ${TEENSY_DEFINITIONS} )
# Macros to wrap add_[executable|library] for seamless Teensy integration
#
function(teensy_add_executable TARGET)
    message("Corepath " ${COREPATH})
    set(ELFTARGET ${TARGET}.o)
    message("mm ${ELFTARGET} ${ARGN} ${TEENSY_SOURCES} ${TEENSY_LIB_SOURCES}")
    add_executable(${ELFTARGET} ${ARGN} ${TEENSY_SOURCES} ${TEENSY_LIB_SOURCES})

    set_target_properties(${ELFTARGET} PROPERTIES COMPILE_FLAGS "-Wall -g -Os -mcpu=${CPU} -mthumb -MMD -fno-exceptions ")
    set_target_properties(${ELFTARGET} PROPERTIES COMPILE_DEFINITIONS "${TEENSY_DEFINITIONS} ${INCLUDE_DIRECTORIES} -D__arm__ ")
    set_target_properties(${ELFTARGET} PROPERTIES INCLUDE_DIRECTORIES "${COREPATH}")
    set_target_properties(${ELFTARGET} PROPERTIES LINK_FLAGS "-O -Wl,--gc-sections,--defsym=__rtc_localtime=0 --specs=nano.specs -mcpu=${CPU} -mthumb -T${LINKER_FILE}")
    #set_target_properties(${ELFTARGET} PROPERTIES LINK_FLAGS "-r")

    add_custom_command(OUTPUT ${TARGET}.hex
            COMMAND ${COMPILERPATH}arm-none-eabi-size ${ELFTARGET}
            COMMAND ${COMPILERPATH}arm-none-eabi-objcopy -O ihex -R .eeprom ${ELFTARGET} ${TARGET}.hex
            DEPENDS ${ELFTARGET}
            COMMENT "Creating HEX file for ${ELFTARGET}")

    add_custom_target(hex ALL DEPENDS ${TARGET}.hex)
endfunction()

macro(import_arduino_library LIB_ROOT LIB_NAME)
    # Check if we can find the library.
    if(NOT EXISTS ${LIB_ROOT})
        message(FATAL_ERROR "Could not find the Arduino library directory ${LIB_ROOT}")
    endif(NOT EXISTS ${LIB_ROOT})
    set(LIB_DIR "${LIB_ROOT}/${LIB_NAME}")
    import_arduino_library_absolute(${LIB_DIR})
endmacro(import_arduino_library)

macro(import_arduino_library_absolute LIB_DIR)
    message(STATUS LIB_DIR ${LIB_DIR})
    if(NOT EXISTS "${LIB_DIR}")
        message(FATAL_ERROR "Could not find the directory for library '${LIB_DIR}'")
    endif(NOT EXISTS "${LIB_DIR}")

    # Add it to the include path.
    include_directories("${LIB_DIR}")
    set(INCLUDE_DIRECTORIES "${INCLUDE_DIRECTORIES}-I${LIB_DIR} ")
    message("include_directories ${LIB_DIR}")

    # Mark source files to be built along with the sketch code.
    file(GLOB_RECURSE SOURCES_CPP ABSOLUTE "${LIB_DIR}/*.cpp")
    foreach(SOURCE_CPP ${SOURCES_CPP})
        set(TEENSY_LIB_SOURCES ${TEENSY_LIB_SOURCES} ${SOURCE_CPP})
        set_source_files_properties(${SOURCE_S} PROPERTIES COMPILE_FLAGS "-std=gnu++0x -felide-constructors -fno-rtti")
        message("SOURCE_CPP ${SOURCE_CPP}")
    endforeach(SOURCE_CPP ${SOURCES_CPP})

    file(GLOB_RECURSE SOURCES_C ABSOLUTE "${LIB_DIR}/*.c")
    foreach(SOURCE_C ${SOURCES_C})
        set(TEENSY_LIB_SOURCES ${TEENSY_LIB_SOURCES} ${SOURCE_C})
        set_source_files_properties(${SOURCE_S} PROPERTIES COMPILE_FLAGS "-nostdlib")
        message("SOURCE_C ${SOURCE_C}")
    endforeach(SOURCE_C ${SOURCES_C})

    file(GLOB_RECURSE SOURCES_S ABSOLUTE "${LIB_DIR}/*.S")
    foreach(SOURCE_S ${SOURCES_S})
        set_property(SOURCE ${SOURCE_S} PROPERTY LANGUAGE C)
        set_source_files_properties(${SOURCE_S} PROPERTIES COMPILE_FLAGS "-x assembler-with-cpp")
        set(TEENSY_LIB_SOURCES ${TEENSY_LIB_SOURCES} ${SOURCE_S})
        message("SOURCE_S ${SOURCE_S}")
    endforeach(SOURCE_S ${SOURCES_S})

endmacro(import_arduino_library_absolute)

macro(teensy_remove_sources LIB_DIR)
    file(GLOB_RECURSE FILES_TO_REMOVE ABSOLUTE "${LIB_DIR}/*.*")
    foreach(FILE_TO_REMOVE ${FILES_TO_REMOVE})
        list(REMOVE_ITEM TEENSY_LIB_SOURCES ${FILE_TO_REMOVE})
        message("REMOVED ${FILE_TO_REMOVE}")
    endforeach(FILE_TO_REMOVE ${FILES_TO_REMOVE})
endmacro(teensy_remove_sources)

set(ARDUINO_LIB_ROOT "${DEPSPATH}")
set(INCLUDE_DIRECTORIES "${INCLUDE_DIRECTORIES}-I${ARDUINO_LIB_ROOT}/Audio/utility ")
import_arduino_library(${ARDUINO_LIB_ROOT} SPI)
import_arduino_library(${ARDUINO_LIB_ROOT} SdFat/src)
import_arduino_library(${ARDUINO_LIB_ROOT} SD/src)
import_arduino_library(${ARDUINO_LIB_ROOT} Audio)
import_arduino_library(${ARDUINO_LIB_ROOT} SerialFlash)
import_arduino_library(${ARDUINO_LIB_ROOT} Wire)
import_arduino_library(${ARDUINO_LIB_ROOT} arm_math/src)

teensy_remove_sources(${ARDUINO_LIB_ROOT}/Audio/extras)
teensy_remove_sources(${ARDUINO_LIB_ROOT}/Audio/examples)
teensy_remove_sources(${ARDUINO_LIB_ROOT}/SPI/examples)
teensy_remove_sources(${ARDUINO_LIB_ROOT}/SerialFlash/examples)
teensy_remove_sources(${ARDUINO_LIB_ROOT}/SerialFlash/util)
teensy_remove_sources(${ARDUINO_LIB_ROOT}/Wire/examples)

teensy_add_executable(teensy-audio-resampler
        main.cpp
        playsdrawresmp.h
        playsdrawresmp.cpp
        playsdwavresmp.h
        playsdwavresmp.cpp)

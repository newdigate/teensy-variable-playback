set(TEENSY_VERSION 41 CACHE STRING "Set to the Teensy version corresponding to your board (30 or 31 allowed)" FORCE)
set(CPU_CORE_SPEED 600000000 CACHE STRING "Set to 24000000, 48000000, 72000000 or 96000000 to set CPU core speed" FORCE) # Derived variables
set(CMAKE_EXE_LINKER_FLAGS "--specs=nosys.specs" CACHE INTERNAL "")
#teensy compiler options
set(COMPILERPATH "/opt/gcc-arm-none-eabi-9-2019-q4-major/bin/")
#set(COMPILERPATH "/Applications/ARM/bin/")

set(BUILD_FOR_TEENSY ON)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
set(CMAKE_C_COMPILER ${COMPILERPATH}arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER ${COMPILERPATH}arm-none-eabi-g++)
set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_C_COMPILER} <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
# variable rate playback for teensy audio library
[![Teensy 4.0](https://img.shields.io/badge/project-4.0-brightgreen.svg?label=Teensy&colorA=555555&colorB=ff8aff&logo=)](https://www.pjrc.com/store/teensy40.html)
[![lib-teensy41](https://github.com/newdigate/teensy-variable-playback/actions/workflows/teensy41_lib.yml/badge.svg)](https://github.com/newdigate/teensy-variable-playback/actions/workflows/teensy41_lib.yml)
[![Ubuntu-x64](https://github.com/newdigate/teensy-variable-playback/workflows/Ubuntu-x64/badge.svg)](https://github.com/newdigate/teensy-variable-playback/actions)
[![MIT license](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![CMake](https://img.shields.io/badge/project-CMake-brightgreen.svg?label=built%20with&colorA=555555&colorB=8a8fff&logo=)](CMakelists.txt)
[![made-for-VSCode](https://img.shields.io/badge/Made%20for-VSCode-1f425f.svg)](https://code.visualstudio.com/)


A set of classes to read and play 16-bit raw mono audio from micro sd-card at variable forward and backward playback rates on teensy
* for best performance, use SDXC UHS 30MB/sec Application Performance Class 2 (A2) class micro sd-card. 
  * [sd classes on wikipedia](https://en.wikipedia.org/wiki/SD_card#cite_ref-93) 

## todo
* stereo  

## requirements
  * x86 / x64 linux unit tests
    * cmake
    * gcc or llvm: c++ standard library (using std::vector, std::function)
    * [newdigate](https://github.com/newdigate)
      * [teensy-x86-stubs](https://github.com/newdigate/teensy-x86-stubs)
      * [teensy-audio-x86-stubs](https://github.com/newdigate/teensy-audio-x86-stubs)
      * [teensy-x86-sd-stubs](https://github.com/newdigate/teensy-x86-sd-stubs)
    * boost unit-test library: ```sudo apt-get install -yq libboost-test-dev```
    
  * teensy variable playback library  
    * cmake
    * [gcc-arm-none-eabi](https://developer.arm.com/-/media/Files/downloads/gnu-rm/9-2019q4/RC2.1): for cross-compiling to arm cortex m7 architecture (for teensy 4.0)
    * github
      * newdigate
        * [teensy-cmake-macros](https://github.com/newdigate/teensy-cmake-macros)
        ```shell
        > git clone https://github.com/newdigate/teensy-cmake-macros.git
        > cd teensy-cmake-macros
        > mkdir cmake-build-debug
        > cd cmake-build-debug
        > cmake ..
        > sudo make install        
        ```

      * PaulStoffregen
        * [cores.git](https://github.com/PaulStoffregen/cores)
        * [Audio.git](https://github.com/PaulStoffregen/Audio)
        * [SD.git @ Juse_Use_SdFat](https://github.com/PaulStoffregen/SD/tree/Juse_Use_SdFat)
        * [Wire.git](https://github.com/PaulStoffregen/Wire)
        * [SPI.git](https://github.com/PaulStoffregen/SPI)
        * [SerialFlash.git](https://github.com/PaulStoffregen/SerialFlash)
        * [arm_math.git](https://github.com/PaulStoffregen/arm_math)
      * greiman
        * [SdFat.git](https://github.com/greiman/SdFat)

![dependencies](docs/dependencies.png)
```dot
graph G {
  graph[rankdir="LR"]
  "teensy variable playback" -- "teensy-cmake-macros" -- "cmake" [label="dev"]
  "teensy-cmake-macros" -- "arm-none-eabi-gcc"  [label="dev"]
  "PaulStoffregen/Audio.git" -- "PaulStoffregen/cores.git"
  "teensy variable playback" -- "PaulStoffregen/Audio.git"
  "PaulStoffregen/Audio.git" -- "PaulStoffregen/SD.git@Juse_Use_SdFat"
  "PaulStoffregen/SD.git@Juse_Use_SdFat" -- "PaulStoffregen/SPI.git"
  "PaulStoffregen/SD.git@Juse_Use_SdFat" -- "greiman/SdFat.git"
  "PaulStoffregen/Audio.git" -- "PaulStoffregen/Wire.git"
  "PaulStoffregen/Audio.git" -- "PaulStoffregen/SerialFlash.git"
  "PaulStoffregen/Audio.git" -- "PaulStoffregen/arm_math.git"
}

```

        
## code structure

| folder | target             | description                                                                                                            |
|--------|--------------------|------------------------------------------------------------------------------------------------------------------------|
| ```examples```    | ```teensy``` | basic example how to use  |
| ```src```    | ```teensy``` / <br/>```test``` | extends teensy audio library<br/> * adds ```AudioPlaySdRawResmp```<br/> * play raw audio at variable forward and backward playback rates     |
| ```test```   | ```test```          | unit tests that run on linux x64 or x86 architecture, <br/> tests certain non-audio code in ```src``` folder|
  
## build and run/debug tests locally on pc
### clone repo
``` sh
> git clone https://github.com/newdigate/teensy-variable-playback.git
> cd teensy-variable-playback
```

* manual configuration steps required:
  * if you just want to compile, and run/debug the tests, you can comment out the ```audio``` subdirectory in the root ```CMakeLists.txt```
    * ```add_subdirectory(audio)``` -> ```#add_subdirectory(audio)```
  * update ```examples/CMakeLists.cmake.in```:
    * set cmake path variables to point to your arm gcc toolchain, teensy cores, and library dependencies
 ``` cmake
set(COMPILERPATH "/Users/xxx/gcc-arm-none-eabi-7-2017-q4-major/bin/")
set(DEPSPATH "/Applications/Teensyduino.app/Contents/Java/hardware/teensy/avr/libraries")
set(COREPATH "/Applications/Teensyduino.app/Contents/Java/hardware/teensy/avr/cores/teensy4/")
```

### build tests on linux
``` sh
> ./build
```

### build tests on win
``` sh
> mkdir cmake-build-debug
> cd cmake-build-debug
> cmake -DCMAKE_BUILD_TYPE=Debug
> make
```

### run tests
``` sh
> cmake-build-debug/test/test_suite1
```

## visual studio code
  * download vs code
    * install plug-in ms-vscode.cpptools
  * open root folder of repository in visual studio code
  * open terminal in visual studio code build, build as above
  * (add breakpoint)
  * launch

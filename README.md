# variable playback rate for teensy audio library
[![C/C++ CI](https://github.com/newdigate/teensy-variable-playback/workflows/C/C++%20CI/badge.svg) ](https://github.com/newdigate/teensy-variable-playback/actions)
[![MIT license](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![made-for-VSCode](https://img.shields.io/badge/Made%20for-VSCode-1f425f.svg)](https://code.visualstudio.com/)
[![made with JetBrains Clion](https://img.shields.io/badge/project-Clion-brightgreen.svg?label=Made%20for&colorA=555555&colorB=ff8a2c&logo=data%3Aimage%2Fsvg%2Bxml%3Bbase64%2CPD94bWwgdmVyc2lvbj0iMS4wIiBlbmNvZGluZz0idXRmLTgiPz48c3ZnIHZlcnNpb249IjEuMSIgaWQ9IkxheWVyXzEiIHhtbG5zPSJodHRwOi8vd3d3LnczLm9yZy8yMDAwL3N2ZyIgeG1sbnM6eGxpbms9Imh0dHA6Ly93d3cudzMub3JnLzE5OTkveGxpbmsiIHg9IjBweCIgeT0iMHB4IiB3aWR0aD0iMTRweCIgaGVpZ2h0PSIxNHB4IiB2aWV3Qm94PSIwIDAgMTQgMTQiIGVuYWJsZS1iYWNrZ3JvdW5kPSJuZXcgMCAwIDE0IDE0IiB4bWw6c3BhY2U9InByZXNlcnZlIj48cmVjdCB4PSIxIiB5PSIxMiIgZmlsbD0iI0ZGRkZGRiIgd2lkdGg9IjciIGhlaWdodD0iMSIvPjxwYXRoIGZpbGw9IiNGRkZGRkYiIGQ9Ik0wLjMsNy4zbDEtMS4xYzAuNCwwLjUsMC44LDAuNywxLjMsMC43YzAuNiwwLDEtMC40LDEtMS4yVjFoMS42djQuN2MwLDAuOS0wLjIsMS41LTAuNywxLjlDNC4xLDguMSwzLjQsOC40LDIuNiw4LjRDMS41LDguNCwwLjgsNy45LDAuMyw3LjN6Ii8%2BPHBhdGggZmlsbD0iI0ZGRkZGRiIgZD0iTTYuOCwxaDMuNGMwLjgsMCwxLjUsMC4yLDEuOSwwLjZjMC4zLDAuMywwLjUsMC43LDAuNSwxLjJsMCwwYzAsMC44LTAuNCwxLjMtMSwxLjZDMTIuNSw0LjgsMTMsNS4zLDEzLDYuMmwwLDBjMCwxLjMtMS4xLDItMi43LDJINi44VjF6IE0xMSwzLjFjMC0wLjUtMC40LTAuNy0xLTAuN0g4LjR2MS41aDEuNUMxMC42LDMuOSwxMSwzLjcsMTEsMy4xTDExLDMuMXogTTEwLjIsNS4zSDguNHYxLjZoMS45YzAuNywwLDEuMS0wLjIsMS4xLTAuOGwwLDBDMTEuNCw1LjYsMTEuMSw1LjMsMTAuMiw1LjN6Ii8%2BPHJlY3QgeD0iMSIgeT0iMTIiIGZpbGw9IiNGRkZGRkYiIHdpZHRoPSI3IiBoZWlnaHQ9IjEiLz48L3N2Zz4%3D)](https://www.jetbrains.com/clion/)

A set of classes to read and play 16-bit raw mono audio from micro sd-card at variable forward and backward playback rates on teensy
* for best performance, use SDXC UHS 30MB/sec Application Performance Class 2 (A2) class micro sd-card. 
  * [sd classes on wikipedia](https://en.wikipedia.org/wiki/SD_card#cite_ref-93) 

## todo
* read .wav files
* improve looping controls, allow xm-tracker like sample looping, 
  * loop start and finish markers, ping ponging, repeating, etc.
* stereo  

## requirements
  * x86 / x64 linux unit tests
    * cmake
    * gcc or llvm: c++ standard library (using std::vector, std::function)
    * boost unit-test library: ```sudo apt-get install -yq libboost-test-dev```
    
  * teensy variable playback library  
    * cmake
    * [gcc-arm-none-eabi](https://developer.arm.com/-/media/Files/downloads/gnu-rm/9-2019q4/RC2.1): for cross-compiling to arm cortex m7 architecture (for teensy 4.0)
    * github
      * PaulStoffregen
        * cores.git
        * Audio.git
          * SD.git
          * Wire.git
          * SPI.git
          * SerialFlash.git
        
## code structure

| folder | target             | description                                                                                                            |
|--------|--------------------|------------------------------------------------------------------------------------------------------------------------|
| ```audio```  | ```teensy```             | extends teensy audio library<br/> * adds ```AudioPlaySdRawResmp```<br/> * play raw audio at variable forward and backward playback rates |
| ```src```    | ```teensy``` / <br/>```x86-tests``` | shared code used by ```audio``` and ```test```                                                                               |
| ```teensy``` | ```x86-tests```          | used by ```test``` <br/> * mock arduino/teensyduino functions <br/> * allows unit-tests to be compiled on x86/x64 architecture  |
| ```test```   | ```x86-tests```          | unit tests that run on linux x64 or x86 architecture, <br/> tests code in ```src``` folder|
  
## build and run/debug tests locally on pc
### clone repo
``` sh
git clone https://github.com/newdigate/teensy-variable-playback.git
cd teensy-variable-playback
```

* manual configuration steps required:
  * if you just want to compile, and run/debug the tests, you can comment out the ```audio``` subdirectory in the root ```CMakeLists.txt```
    * ```add_subdirectory(audio)``` -> ```#add_subdirectory(audio)```
  * update ```audio/CMakeLists.txt```:
    * set cmake path variables to point to your arm gcc toolchain, teensy cores, and library dependencies
 ``` cmake
set(COMPILERPATH "/Users/xxx/gcc-arm-none-eabi-7-2017-q4-major/bin/")
set(DEPSPATH "/Applications/Teensyduino.app/Contents/Java/hardware/teensy/avr/libraries")
set(COREPATH "/Applications/Teensyduino.app/Contents/Java/hardware/teensy/avr/cores/teensy4/")
```

### build tests on linux
``` sh
./build
```
### build tests on win
``` sh
mkdir cmake-build-debug
cd cmake-build-debug
cmake -DCMAKE_BUILD_TYPE=Debug
make
```
### run tests
```
cmake-build-debug/test/test_suite1
```

## visual studio code
  * download vs code
    * install plug-in ms-vscode.cpptools
  * open root folder of repository in visual studio code
  * open terminal in visual studio code build, build as above
  * (add breakpoint)
  * launch

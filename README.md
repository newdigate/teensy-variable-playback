# variable rate playback for teensy audio library
[![Teensy 4.1](https://img.shields.io/badge/project-4.1-brightgreen.svg?label=Teensy&colorA=555555&colorB=ff8aff&logo=)](https://www.pjrc.com/store/teensy41.html)
[![lib-teensy41](https://github.com/newdigate/teensy-variable-playback/actions/workflows/teensy41_lib.yml/badge.svg)](https://github.com/newdigate/teensy-variable-playback/actions/workflows/teensy41_lib.yml)
[![Ubuntu-x64](https://github.com/newdigate/teensy-variable-playback/workflows/Ubuntu-x64/badge.svg)](https://github.com/newdigate/teensy-variable-playback/actions)
[![MIT license](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![CMake](https://img.shields.io/badge/project-CMake-brightgreen.svg?label=built%20with&colorA=555555&colorB=8a8fff&logo=)](CMakelists.txt)
[![made-for-VSCode](https://img.shields.io/badge/Made%20for-VSCode-1f425f.svg)](https://code.visualstudio.com/)
[![Contributors](https://img.shields.io/github/contributors/newdigate/teensy-variable-playback)](https://github.com/newdigate/teensy-variable-playback/graphs/contributors) 
[![Commits](https://img.shields.io/github/commit-activity/m/newdigate/teensy-variable-playback)](https://github.com/newdigate/teensy-variable-playback/graphs/contributors)
![s](https://img.shields.io/badge/dynamic/json?color=%23e85b46&label=Patreon&query=data.attributes.patron_count&suffix=%20patrons&url=https%3A%2F%2Fwww.patreon.com%2Fapi%2Fcampaigns%2F4105381)

play 16-bit PCM raw or wav audio samples at variable playback rates on teensy
* **Note** : this library only works with signed 16-bit integer samples. Floating point samples will not play. 

* for best performance, use SDXC UHS 30MB/sec Application Performance Class 2 (A2) class micro sd-card. 
  * [sd classes on wikipedia](https://en.wikipedia.org/wiki/SD_card#cite_ref-93) 

## updates
* 25/08/2021: v1.0.12: Skip over RIFF tags in .wav header
* 12/08/2021: v1.0.11: When playing a mono sample, transmit on both channels (credit to @atoktoto) 
* 28/07/2021: v1.0.10: Fix issues when starting playback in reverse
* 23/07/2021: v1.0.9: Fix issue which crashes teensy when playing multiple files from SD card using array of filenames
* 21/07/2021: v1.0.8: **Breaking changes** 
  * ```AudioPlaySdRawResmp``` and ```AudioPlaySdWaveResmp``` merged into a single class ```AudioPlaySdResmp```
  * ```play(...)``` method changed to ```playRaw(...)``` and ```playWav(...)```, specify number of channels in parameters of playRaw
* 13/07/2021: v1.0.7: added multi-channel resampling
* 07/07/2021: v1.0.6: changed to using optimised floating point interpolation, sounds much better
* 30/06/2021: v1.0.5: Optimised quadratic interpolation to use fixed pipeline of 4 samples and use integers instead of floating point
* 25/06/2021: Quadratic interpolation is now working, but is disabled by default

## contents
* [code structure](#code-structure)
* [requirements](#requirements)
* [usage](#usage)
* [example usage](#example-usage)

## code structure
| folder | target             | description                                                                                                            |
|--------|--------------------|------------------------------------------------------------------------------------------------------------------------|
| ```examples```    | ```teensy``` | basic example how to use  |
| ```extras```    | ```linux``` | some utils to make life easier  |
| ```src```    | ```teensy``` / ```linux``` | extends teensy audio library<br/> * adds ```AudioPlaySdResmp``` <br/> * adds ```AudioPlayArrayResmp```   |
| ```test```   | ```linux```          | unit tests that run on linux |

## requirements
<details>
  <summary>teensy 3.x & 4.x boards</summary>
 
  
<details>
  <summary>with Teensyduino</summary>  

```Teensyduino```[^](https://www.pjrc.com/teensy/teensyduino.html)
* This library is built on top of teensy audio library,  intended for use with Teensy 3.x and Teensy 4.x boards.
* Install using arduino/teensyduino library manager gui - search ```TeensyVariablePlayback```
  * ![install using arduino library manager](docs/InstallArduino.gif)
</details>
    
  
<details>
  <summary>without Teensyduino (for development)</summary>  

```cmake``` ```gcc-arm-none-eabi```[^](https://developer.arm.com/-/media/Files/downloads/gnu-rm/9-2019q4/RC2.1) ```teensy-cmake-macros```[^](https://github.com/newdigate/teensy-cmake-macros) ```cores```[^](https://github.com/PaulStoffregen/cores) ```Audio```[^](https://github.com/PaulStoffregen/Audio) ```SD```[^](https://github.com/PaulStoffregen/SD/tree/Juse_Use_SdFat) ```Wire```[^](https://github.com/PaulStoffregen/Wire) ```SPI```[^](https://github.com/PaulStoffregen/SPI) ```SerialFlash```[^](https://github.com/PaulStoffregen/SerialFlash) ```arm_math```[^](https://github.com/PaulStoffregen/arm_math) ```SDFat```[^](https://github.com/greiman/SdFat)
* using [teensy-cmake-macros](https://github.com/newdigate/teensy-cmake-macros), this library can be compiled for teensy 3 and 4 boards without needing Teensyduino. This is mainly used to build the library when a commit is pushed, to verify there are no compile errors. 
  
  
<details>
  <summary>dependencies (click to expand image) </summary>
  
![dependencies](docs/dependencies.png)
  
<details>
  <summary>graphvis (click to expand) </summary>
  
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
</details>
  
</details>  
  
  
  
</details>
  
  

  
</details>

<details>
  <summary>linux</summary>
  
```cmake``` ```gcc or llvm``` ```teensy-x86-stubs```[^](https://github.com/newdigate/teensy-x86-stubs) ```teensy-audio-x86-stubs```[^](https://github.com/newdigate/teensy-audio-x86-stubs) ```teensy-x86-sd-stubs```[^](https://github.com/newdigate/teensy-x86-sd-stubs) ```boost-test``` 

By using stub libraries, we can compile teensy code to native device architecture. To a certain extent, this allows sketches and libraries to be developed, emulated, debugged and unit-tested using linux, on your local device or a build server. In this case I have a few basic tests for the ResamplingSdReader class.
  * install boost unit-test library: 
    * linux: ```sudo apt-get install -yq libboost-test-dev```
    * macos: ```brew install boost```

</details>  
  

## usage 
<details>
  <summary>Using with Teensyduino</summary>
  
* To install the library, use the library manager in Teensyduino (search for ```TeensyVariablePlayback```). Teensyduino should already have all the necessary dependencies pre-installed. 

* Have a look at the examples in the file menu to get started...
</details>

<details>
  <summary>Developing with vscode</summary>

  * [Visual Studio Code](https://code.visualstudio.com)
  
### clone repo
``` sh
> git clone https://github.com/newdigate/teensy-variable-playback.git
> cd teensy-variable-playback
```

## teensy build
You don't need to download or install Teensyduino or Arduino to build the library or examples. Just clone the cores library and any dependencies to a common folder, denoted by ```DEPSPATH``` (in this case ```/home/nic/teensy_libraries```). 
<details>
  <summary>clone dependencies (click to expand) </summary>
  
``` sh
 > cd /home/nic/teensy_libraries
 > git clone https://github.com/PaulStoffregen/cores.git
 > git clone https://github.com/PaulStoffregen/Audio.git
 > git clone -b Juse_Use_SdFat https://github.com/PaulStoffregen/SD.git 
 > git clone https://github.com/PaulStoffregen/Wire.git
 > git clone https://github.com/PaulStoffregen/SPI.git
 > git clone https://github.com/PaulStoffregen/SerialFlash.git
 > git clone https://github.com/PaulStoffregen/arm_math.git
 > git clone https://github.com/greiman/SdFat.git
```
  
</details>

<details>
  <summary>update COMPILERPATH and DEPSPATH in cmake/toolchains/teensy41.cmake</summary>
  
``` cmake
set(COMPILERPATH "/Applications/Arm/bin/")
set(DEPSPATH "/home/nic/teensy_libraries")
set(COREPATH "${DEPSPATH}/cores/teensy4/")
```

</details>

<details>
  <summary>build hex file</summary>

  * If you run the commands below from the root repository directory, it will build the teensy-variable-playback library and all the examples. 
  * If you run them from a sub-directory, it will build everything under the sub-directory. (You might need to adjust relative path in ```-DCMAKE_TOOLCHAIN_FILE:FILEPATH``` below)
``` sh
> cd /home/nic/teensy-variable-playback
> mkdir cmake-build-debug
> cd cmake-build-debug
> cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE:FILEPATH="../cmake/toolchains/teensy41.cmake" ..
> make
```

</details>

## linux build
### build tests on linux
``` sh
> ./build-linux.sh
```

### build tests on win
``` sh
> mkdir cmake-build-debug
> cd cmake-build-debug
> cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE:FILEPATH="../cmake/toolchains/linux.cmake" ..
> make
```

### run tests
``` sh
> cmake-build-debug/test/test_suite1
```

## visual studio code
  * download vs code
    * required extensions
      * ms-vscode.cpptools
    * optional extensions
      * ms-vscode.cmake-tools
      * hbenl.vscode-test-explorer
      * ms-vscode.test-adapter-converter
      * nicnewdigate.boost-test-adapter-debug
  * open root folder of repository in visual studio code
  * open terminal in visual studio code build, build as above
  * (add breakpoint)
  * launch

</details>

## example usage

<details>
  <summary>example (click to expand) </summary>
  
```c++
#include <Arduino.h>
#include <Audio.h>
#include <TeensyVariablePlayback.h>

// GUItool: begin automatically generated code
AudioPlayArrayResmp      rraw_a1;        //xy=321,513
AudioOutputI2S           i2s1;           //xy=675,518
AudioConnection          patchCord1(rraw_a1, 0, i2s1, 0);
AudioConnection          patchCord2(rraw_a1, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=521,588
// GUItool: end automatically generated code

unsigned char kick_raw[] = {
  // ... little-endian 16-bit mono 44100 raw data, generated using linux cmd 'xxd -i kick.raw', raw file saved in Audacity
  0x99, 0x02, 0xd7, 0x02, 0xfa, 0x02, 0x5f, 0x03, 0xc1, 0x03, 0x2a, 0x04,
  0xad, 0x04, 0xa5, 0x05, 0x76, 0x06, 0x2f, 0x07, 0x9e, 0x07, 0xe2, 0x07,
  0x43, 0x08, 0x92, 0x08, 0xb2, 0x08, 0xe8, 0x08, 0x16, 0x09, 0xda, 0x08,
  // ... continued ... 
};
unsigned int kick_raw_len = 6350; // length in bytes == numsamples * 2

void setup() {
    AudioMemory(20);
    sgtl5000_1.enable();
    sgtl5000_1.volume(0.5f, 0.5f);
    rraw_a1.setPlaybackRate(0.5);
    rraw_a1.enableInterpolation(true);  
}

void loop() {
    if (!rraw_a1.isPlaying()) {
        delay(1000);
        rraw_a1.playRaw((int16_t *)kick_raw, kick_raw_len/2, 1);  //note: we give number of samples - NOT number of bytes!!!! 1 is for mono (2 for stereo, etc)
    }
}
```
  
</details>  

# credits
* convert boost test report to junit xml format: [Stuart Lange](https://stackoverflow.com/a/2975928/4634140)

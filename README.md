# resampling sdreader [![C/C++ CI](https://github.com/newdigate/teensy-resampling-sdreader/workflows/C/C++%20CI/badge.svg) ](https://github.com/newdigate/teensy-resampling-sdreader/actions)
* class to read audio from sd card at variable forward and backward playback rates on teensy  

## requirements
  * cmake
  * gcc or llvm: c++ standard library (using std::vector, std::function)
  * boost unit-test library: `sudo apt-get install -yq libboost-test-dev
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
| audio  | teensy             | extends teensy audio library,<br/> * adds AudioPlaySdRawResmp<br/>    * play raw audio at variable forward and backward playback rates |
| src    | teensy / <br/>x86-tests | shared code used by audio and x86-tests                                                                               |
| teensy | x86-tests          | used by x86-tests, provides teensy-core stubs to allow teensy code to be compiled on x86 architecture                  |
| test   | x86-tests          | unit tests that run on linux x64 or x86 architecture                                                                        |
  
## build and run/debug tests locally on pc
### clone repo
``` sh
git clone https://github.com/newdigate/teensy-resampling-sdreader.git
cd teensy-resampling-sdreader
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

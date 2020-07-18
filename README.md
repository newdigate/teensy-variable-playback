# resampling sdreader [![C/C++ CI](https://github.com/newdigate/teensy-resampling-sdreader/workflows/C/C++%20CI/badge.svg) ](https://github.com/newdigate/teensy-resampling-sdreader/actions)
* class to read audio from sd card at variable forward and backward playback rates on teensy  

## requirements
  * cmake, 
  * boost unit-test library 

## build, run tests on x86 
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
cmake-build-debug/test_x86/test_resampling_sdreader
```

## visual studio code
  * download vs code
    * install plug-in ms-vscode.cpptools
  * open root folder of repository in visual studio code
  * open terminal in visual studio code build, build as above
  * (add breakpoint)
  * launch

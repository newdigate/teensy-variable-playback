# teensy-resampling-sdreader
(experimental) class to read resampled raw audio from sd card using teensy  

this is a work in progress, please bare with us...

## requirements
  * cmake

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

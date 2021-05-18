#!/bin/bash
cd examples
rm -rf cmake-build-debug
mkdir cmake-build-debug
cd cmake-build-debug
cmake -DCMAKE_BUILD_TYPE=Debug -DOPT_BUILD_FOR_LINUX=OFF ..
make
#!/bin/bash
mkdir -p build && cd build
cmake ..
make
export DYLD_LIBRARY_PATH=../build/_deps/sdl3-build:$DYLD_LIBRARY_PATH
./2DEngine

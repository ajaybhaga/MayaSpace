#!/bin/bash
./cmake_clean.sh
cmake -DCMAKE_BUILD_TYPE=Debug -DVIDEO_WAYLAND:BOOL=OFF .
make -j9
gdb ./bin/MayaSpace

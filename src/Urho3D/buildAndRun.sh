#!/bin/bash
./cmake_clean.sh
cmake -DCMAKE_BUILD_TYPE=Debug .
make -j9
gdb ./bin/MayaSpace

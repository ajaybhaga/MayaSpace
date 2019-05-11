#!/bin/bash
cmake -DCMAKE_BUILD_TYPE=Debug .
make -j9
./bin/MayaSpace

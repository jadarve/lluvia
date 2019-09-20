#!/bin/bash

# C++ libraries
cd /home/lluvia
mkdir build
cd build
cmake -DCMAKE_SYSTEM_NAME=Linux -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Release ..
make -j

# python package
cd /home/lluvia
cd python
CC=clang python3 setup.py build

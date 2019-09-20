.. _CI:

Continuous Integration
======================

Docker Image
============

    docker build ci --tag=jadarve/lluvia:build


Local Build
===========

    # interactive shell
    docker run --entrypoint='' -it --mount type=bind,source="$(pwd)",target="/home/lluvia" jadarve/lluvia:build /bin/bash

    # automatic build
    docker run --user $(id -u):$(id -g) --mount type=bind,source="$(pwd)",target="/home/lluvia" jadarve/lluvia:build

Inside Docker
=============
    
    # C++ libraries
    cd /home/lluvia
    mkdir build
    cd build
    cmake -DCMAKE_SYSTEM_NAME=Linux -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Release ..
    make -j

    # Python package
    cd /home/lluvia
    cd python
    CC=clang python3 setup.py build

#!/usr/bin/env bash

CC=clang bazel build //cpp/core:core_cc_library
CC=clang bazel build @lua//:lua_cc_library
CC=clang python3 setup.py build

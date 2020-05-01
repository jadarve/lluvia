"""
"""

load("//bazel/cython:macros.bzl", _pyx_library = "pyx_library")
load("//bazel/cython:rules.bzl", _cy_compile = "cy_compile")

pyx_library = _pyx_library
cy_compile = _cy_compile

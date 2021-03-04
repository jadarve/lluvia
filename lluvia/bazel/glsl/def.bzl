"""
"""

load("@lluvia//lluvia/bazel/glsl:rules.bzl",
    _glsl_shader = "glsl_shader",
    _glsl_header_library = "glsl_header_library")

glsl_shader = _glsl_shader
glsl_header_library = _glsl_header_library

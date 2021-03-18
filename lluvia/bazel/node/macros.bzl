"""
"""

load("@lluvia//lluvia/bazel/glsl:def.bzl", "glsl_shader")
load("@rules_pkg//:pkg.bzl", "pkg_zip")
load("@rules_pkg//experimental:pkg_filegroup.bzl", "pkg_filegroup")

def ll_node(
        name,
        shader,
        builder,
        deps = None,
        visibility = None):
    """
    Declares a new node

    Args:
        name:
        shader:
        builder:
        deps:
        visibility:
    """

    shader_name = name + "_shader"

    glsl_shader(
        name = shader_name,
        shader = shader,
        deps = deps,
        visibility = visibility,
    )

    pkg_filegroup(
        name = name,
        srcs = [
            shader_name,
            builder,
        ],
        visibility = visibility,
    )

    native.filegroup(
        name = name + "_runfiles",
        srcs = [
            shader_name,
            builder,
        ],
        visibility = visibility,
    )

def ll_node_library(
        name,
        nodes = [],
        strip_prefix = "",
        visibility = None):
    """
    Declares a node library

    Args:
        name: name of the library
        nodes: list of ll_node targets
        strip_prefix:
        visibility: library visibility
    """

    pkg_zip(
        name = name,
        strip_prefix = strip_prefix,
        srcs = nodes,
        visibility = visibility,
    )

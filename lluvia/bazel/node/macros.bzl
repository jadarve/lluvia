"""
"""

load("@rules_vulkan//glsl:defs.bzl", "glsl_shader")
load("@rules_pkg//:pkg.bzl", "pkg_zip")
load("@rules_pkg//experimental:pkg_filegroup.bzl", "pkg_filegroup")

def ll_node(
        name,
        builder,
        shader=None,
        deps = None,
        visibility = None):
    """
    Declares a new node

    Args:
        name:
        builder:
        shader:
        deps:
        visibility:
    """

    srcs = [builder]

    if shader != None:
        shader_name = name + "_shader"

        glsl_shader(
            name = shader_name,
            shader = shader,
            deps = deps,
            visibility = visibility,
        )

        srcs.append(shader_name)

    pkg_filegroup(
        name = name,
        srcs = srcs,
        visibility = visibility,
    )

    native.filegroup(
        name = name + "_runfiles",
        srcs = srcs,
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

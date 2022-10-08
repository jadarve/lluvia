"""
"""

load("@rules_vulkan//glsl:defs.bzl", "glsl_shader")
load("@ll_rules_pkg//:pkg.bzl", "pkg_zip")
load("@ll_rules_pkg//pkg:mappings.bzl", "pkg_files", "pkg_filegroup")

def ll_node(
        name,
        builder,
        archivePath = "",
        shader=None,
        deps = None,
        visibility = None):
    """
    Declares a new node

    Args:
        name:
        builder:
        archivePath:
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

    pkg_files(
        name = name + "_pkg_files",
        srcs = srcs,
        prefix = archivePath,
    )

    pkg_filegroup(
        name = name,
        srcs = [name + "_pkg_files"],
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

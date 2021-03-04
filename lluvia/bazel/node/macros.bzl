"""
"""

load("@lluvia//lluvia/bazel/glsl:def.bzl", "glsl_shader")
load("@rules_pkg//:pkg.bzl", "pkg_zip")
load("@rules_pkg//experimental:pkg_filegroup.bzl", "pkg_filegroup")

def ll_node(
    name,
    shader,
    builder,
    prefix = "",
    deps = None,
    visibility = None):
    """
    Declares a new node

    Args:
        name:
        shader:
        builder:
        prefix:
        deps:
        visibility:
    """

    shader_name = name + "_shader"

    glsl_shader(
        name = shader_name,
        shader = shader,
        deps = deps,
        visibility = visibility
    )

    pkg_filegroup (
        name = name,
        srcs = [
            shader_name,
            builder,
        ],
        prefix = prefix,
        visibility = visibility,
    )


def ll_node_library(
    name,
    nodes = [],
    prefix = "",
    visibility = None):
    """
    Declares a node library

    Args:
        name: name of the library
        nodes: list of ll_node targets
        prefix:
        visibility: library visibility
    """

    # FIXME: even though srcs are pkg_filegroup targets, pkg_zip is not recognizing
    #        the prefix or strip_prefix attributes of the pkg_filegroup.
    #        See https://github.com/bazelbuild/rules_pkg/issues/30
    pkg_zip(
        name = name,
        extension = "zip",
        package_dir = prefix,
        srcs = nodes,
        visibility = visibility,
    )
    
    # FIXME: pkg_zip generated archive is not accessible directly in the tests
    #        I need to wrap it in a filegroup so that it appears in the runfiles
    #        of targets.
    native.filegroup(
        name = name + ".runfiles",
        srcs = [
            ":" + name,
        ],
        visibility = visibility,
    )

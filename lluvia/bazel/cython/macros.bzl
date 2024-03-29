"""
"""

load("@rules_python//python:defs.bzl", "py_library")
load("@rules_cc//cc:defs.bzl", "cc_binary")
load("//lluvia/bazel/cython:rules.bzl", "cy_compile")

def pyx_library(
        name,
        deps = [],
        py_deps = [],
        srcs = [],
        imports = [],
        testonly = False,
        srcs_version = "PY2AND3",
        directives = [
            "--3str",
            "--cplus",
        ],
        copts = [],
        linkopts = [],
        **kwargs):
    """Compiles a group of .pyx / .pxd / .py files.

    Taken from https://github.com/tensorflow/tensorflow/blob/master/tensorflow/core/platform/default/build_config.bzl

    First runs Cython to create .cpp files for each input .pyx or .py + .pxd
    pair. Then builds a shared object for each, passing "deps" to each cc_binary
    rule (includes Python headers by default). Finally, creates a py_library rule
    with the shared objects and any pure Python "srcs", with py_deps as its
    dependencies; the shared objects can be imported like normal Python files.

    Args:
        name: str. Name for the rule.
        deps: list of labels.
            C/C++ dependencies of the Cython (e.g. Numpy headers).
        py_deps: list of labels.
            Pure Python dependencies of the final library.
        srcs: list of files.
            .py, .pyx, or .pxd files to either compile or pass through.
        imports: list of strings
            List of import paths passed to the py_library target.
        testonly: boolean.
            Defaults to False.
        srcs_version: str.
            Defaults to 'PY2AND3'
        directives: list of str.
            Directives passed to the cython compiler. Defaults to
            ["--3str", "--cplus"].
        copts: list of strings.
            Parameters passed to the cc_binary rule.
        linkopts: list of strings.
            Parameters passed to the cc_binary rule.
        **kwargs: Extra keyword arguments passed to the py_library.
    """

    # First filter out files that should be run compiled vs. passed through.
    py_srcs = []
    pyx_srcs = []
    pxd_srcs = []
    py_init = []

    for src in srcs:
        if src.endswith("__init__.py"):
            py_init.append(src)
            py_srcs.append(src)
        elif src.endswith(".pyx") or (src.endswith(".py") and src[:-3] + ".pxd" in srcs):
            pyx_srcs.append(src)
        elif src.endswith(".py"):
            py_srcs.append(src)
        else:
            pxd_srcs.append(src)

    # Invoke cython to produce the shared object libraries.
    shared_objects_linux = []
    shared_objects_windows = []
    for filename in pyx_srcs:
        filename_noextension = filename[:-4]  # remove .pyx

        cy_compile_target = filename_noextension + "_cy_compile"
        cy_compile(
            name = cy_compile_target,
            pyx = filename,
            headers = pxd_srcs + py_init,
            directives = directives,
        )

        cc_binary(
            name = filename_noextension + "_library",
            srcs = [cy_compile_target],
            deps = deps,
            linkshared = True,
            testonly = testonly,
            copts = copts,
            linkopts = linkopts,
            visibility = ["//visibility:public"],
        )

        # for Windows, need to rename the extension of the cc_binary from .dll to .pyd
        shared_object_name = filename_noextension

        cp_input = filename_noextension + "_library"

        ########################################
        # Generate shared libraries for Linux
        ########################################
        cp_output_linux = filename_noextension + ".so"
        shared_object_name_linux = shared_object_name + "_linux"
        native.genrule(
            name = shared_object_name_linux,
            srcs = [cp_input],
            outs = [cp_output_linux],
            cmd = "cp $(location {0}) $(location {1})".format(cp_input, cp_output_linux),
        )

        shared_objects_linux.append(shared_object_name_linux)

        ########################################
        # Generate shared libraries for Windows
        ########################################
        cp_output_windows = filename_noextension + ".pyd"
        shared_object_name_windows = shared_object_name + "_windows"
        native.genrule(
            name = shared_object_name_windows,
            srcs = [cp_input],
            outs = [cp_output_windows],
            cmd = "cp $(location {0}) $(location {1})".format(cp_input, cp_output_windows),
            cmd_bat = "copy $(location {0}) $(location {1})".format(cp_input, cp_output_windows),
        )

        shared_objects_windows.append(shared_object_name_windows)

    shared_objects = select({
        "@lluvia//:windows": shared_objects_windows,
        # TODO: MacOS
        "//conditions:default": shared_objects_linux,
    })

    # Now create a py_library with these shared objects as data.
    py_library(
        name = name,
        srcs = py_srcs,
        deps = py_deps,
        imports = imports,
        srcs_version = srcs_version,
        data = pxd_srcs + shared_objects,
        testonly = testonly,
        **kwargs
    )

    native.filegroup(
        name = name + "_files",
        srcs = py_srcs + pxd_srcs + shared_objects,
        data = shared_objects,
        visibility = ["//visibility:public"],
    )

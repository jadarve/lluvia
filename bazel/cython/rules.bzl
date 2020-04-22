load("@rules_python//python:defs.bzl", "py_library")
load("@rules_cc//cc:defs.bzl", "cc_binary")


def pyx_library(
        name,
        deps = [],
        py_deps = [],
        srcs = [],
        testonly = False,
        srcs_version = "PY2AND3",
        copts = [],
        linkopts = [],
        cy_include_prefix = '',
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
        testonly: boolean.
            Defaults to False.
        srcs_version: str.
            Defaults to 'PY2AND3'
        copts: list of strings.
            Parameters passed to the cc_binary rule.
        linkopts: list of strings.
            Parameters passed to the cc_binary rule.
        cy_include_prefix: str.
            include prefix added to cython -I argument. Set it to the relative
            path within your project where you define the pyx_library
        **kwargs: Extra keyword arguments passed to the py_library.
    """

    # First filter out files that should be run compiled vs. passed through.
    py_srcs = []
    pyx_srcs = []
    pxd_srcs = []
    py_init = []

    include_paths = list()

    for src in srcs:

        inc = '/'.join(src.split('/')[:-1])

        if inc not in include_paths:
            include_paths.append(inc)

        if src.endswith(".pyx") or (src.endswith(".py") and src[:-3] + ".pxd" in srcs):
            pyx_srcs.append(src)
        elif src.endswith(".py"):
            py_srcs.append(src)
        else:
            pxd_srcs.append(src)

        if src.endswith("__init__.py"):
            py_init.append(src)

    # compute include paths
    includes = ''
    for inc in include_paths:
        includes += ' -I {0}/{1}'.format(cy_include_prefix, inc)

    # Invoke cython to produce the shared object libraries.
    for filename in pyx_srcs:

        cmd = "PYTHONHASHSEED=0 $(location @cython//:cython_binary) {0} --3str --cplus $(location {1}) --output-file $(OUTS)".format(includes, filename)

        # this can be a rule
        native.genrule(
            name = filename + "_cython_translation",
            srcs = [filename] + pxd_srcs,
            outs = [filename.split(".")[0] + ".cpp"],
            # Optionally use PYTHON_BIN_PATH on Linux platforms so that python 3
            # works. Windows has issues with cython_binary so skip PYTHON_BIN_PATH.
            cmd = cmd,
            testonly = testonly,
            tools = ["@cython//:cython_binary"] + pxd_srcs + py_init,
        )

    shared_objects = []
    for src in pyx_srcs:
        stem = src.split(".")[0]
        shared_object_name = stem + ".so"
        cc_binary(
            name = shared_object_name,
            srcs = [stem + ".cpp"],
            deps = deps,
            linkshared = True,
            testonly = testonly,
            copts = copts,
            linkopts = linkopts,
        )
        shared_objects.append(shared_object_name)

    # Now create a py_library with these shared objects as data.
    py_library(
        name = name,
        srcs = py_srcs,
        deps = py_deps,
        srcs_version = srcs_version,
        data = shared_objects + pxd_srcs,
        testonly = testonly,
        **kwargs
    )

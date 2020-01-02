"""
Taken from https://github.com/tensorflow/tensorflow/blob/master/tensorflow/core/platform/default/build_config.bzl

Compiles a group of .pyx / .pxd / .py files.
    First runs Cython to create .cpp files for each input .pyx or .py + .pxd
    pair. Then builds a shared object for each, passing "deps" to each cc_binary
    rule (includes Python headers by default). Finally, creates a py_library rule
    with the shared objects and any pure Python "srcs", with py_deps as its
    dependencies; the shared objects can be imported like normal Python files.
    Args:
      name: Name for the rule.
      deps: C/C++ dependencies of the Cython (e.g. Numpy headers).
      py_deps: Pure Python dependencies of the final library.
      srcs: .py, .pyx, or .pxd files to either compile or pass through.
      **kwargs: Extra keyword arguments passed to the py_library.
"""

load("@rules_python//python:defs.bzl", "py_binary")

# load("@python_deps//:requirements.bzl", "requirement")


def _pyx_library(ctx):
    

    # First filter out files that should be run compiled vs. passed through.
    py_srcs = list()
    pyx_srcs = list()
    pxd_srcs = list()

    for src in ctx.files.srcs:

        print(dir(src))

        filename = src.basename
        if filename.endswith(".pyx") or (filename[:-len(".pyx")] + ".pxd") in ctx.attr.srcs:
            pyx_srcs.append(src)
        
        elif filename.endswith(".py"):
            py_srcs.append(src)
        
        else:
            pxd_srcs.append(src)

        if filename.endswith("__init__.py"):
            pxd_srcs.append(src)

    # Invoke cython to produce the shared object libraries.
    outputs = list()
    shared_objects = list()
    for src in pyx_srcs:
        filename = src.basename.split(".")[0] + ".cpp"
        
        out = ctx.actions.declare_file(filename, sibling=src)
        outputs.append(out)

        args = ctx.actions.args()
        args.add("--cplus", src.path)
        args.add("--output-file", out.path)

        cmd = "cython $@"

        ctx.actions.run_shell(
            inputs = [src],
            outputs = [out],
            arguments = [args],
            command = cmd,
            progress_message = "running cython",
            mnemonic = "cyhton"
        )

        # native.genrule(
        #     name = src.basename + "_cython_translation",
        #     srcs = [src],
        #     outs = [filename],
        #     # Optionally use PYTHON_BIN_PATH on Linux platforms so that python 3
        #     # works. Windows has issues with cython_binary so skip PYTHON_BIN_PATH.
        #     # cmd = "PYTHONHASHSEED=0 $(location @cython//:cython_binary) --cplus $(SRCS) --output-file $(OUTS)",
        #     cmd = "PYTHONHASHSEED=0 cython --cplus $(SRCS) --output-file $(OUTS)",
        #     testonly = ctx.attr.testonly,
        #     tools = ["@cython//:cython_binary"] + pxd_srcs,
        # )

    
        stem = src.basename.split(".")[0]
        shared_object_name = stem + ".so"
        native.cc_binary(
            name = shared_object_name,
            srcs = out.basename,
            deps = ctx.attr.deps,
            linkshared = True,
            testonly = ctx.attr.testonly,
        )
        shared_objects.append(shared_object_name)
    
    print(outputs)
    return [
        DefaultInfo(files=depset(outputs))
    ]

    # for src in srcs:
    #     if src.endswith(".pyx") or (src.endswith(".py") and
    #                                 src[:-3] + ".pxd" in srcs):
    #         pyx_srcs.append(src)
    #     elif src.endswith(".py"):
    #         py_srcs.append(src)
    #     else:
    #         pxd_srcs.append(src)
    #     if src.endswith("__init__.py"):
    #         pxd_srcs.append(src)

    # # Invoke cython to produce the shared object libraries.
    # for filename in pyx_srcs:
    #     native.genrule(
    #         name = filename + "_cython_translation",
    #         srcs = [filename],
    #         outs = [filename.split(".")[0] + ".cpp"],
    #         # Optionally use PYTHON_BIN_PATH on Linux platforms so that python 3
    #         # works. Windows has issues with cython_binary so skip PYTHON_BIN_PATH.
    #         cmd = "PYTHONHASHSEED=0 $(location @cython//:cython_binary) --cplus $(SRCS) --output-file $(OUTS)",
    #         testonly = testonly,
    #         tools = ["@cython//:cython_binary"] + pxd_srcs,
    #     )

    ## TODO
    # shared_objects = []
    # for src in pyx_srcs:
    #     stem = src.split(".")[0]
    #     shared_object_name = stem + ".so"
    #     native.cc_binary(
    #         name = shared_object_name,
    #         srcs = [stem + ".cpp"],
    #         deps = deps + ["@org_tensorflow//third_party/python_runtime:headers"],
    #         linkshared = 1,
    #         testonly = testonly,
    #     )
    #     shared_objects.append(shared_object_name)

    # # Now create a py_library with these shared objects as data.
    # native.py_library(
    #     name = name,
    #     srcs = py_srcs,
    #     deps = py_deps,
    #     srcs_version = srcs_version,
    #     data = shared_objects,
    #     testonly = testonly,
    #     **kwargs
    # )


pyx_library = rule(
    implementation = _pyx_library,
    attrs = {
        "srcs": attr.label_list(allow_files=[".pyx", ".pxd", ".py"]),
        "deps": attr.label_list(),
        "py_deps": attr.label_list(),
    }   
)

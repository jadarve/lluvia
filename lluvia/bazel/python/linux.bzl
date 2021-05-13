"""
"""

def _impl(repository_ctx):
  
    result = repository_ctx.execute(
        ["python3",
         "-c", 
         "import platform; from distutils.sysconfig import get_config_var; print(get_config_var('prefix')); print('.'.join(platform.python_version_tuple()[:2]))"
        ])

    # In a typical linux install, prefix equals to "/usr"
    prefix, version = result.stdout.splitlines()
    
    # creates a symbolink link of the system's Python into this repo
    repository_ctx.symlink(prefix, "python3")

    template = """
cc_library(
    name = "python3-lib",
    srcs = ["python3/lib/python<VERSION>/config-<VERSION>m-x86_64-linux-gnu/libpython<VERSION>.so"],
    hdrs = glob(["python3/include/python<VERSION>/*.h"]),
    includes = ["python3/include/python<VERSION>"],
    strip_include_prefix = "python3/include/python<VERSION>",
    visibility = ["//visibility:public"]
)
"""

    # this replaces the "<VERSION>" string in the template with the actual Python3 version found (e.g., 3.6)
    repo_cotent = template.replace("<VERSION>", version)

    repository_ctx.file("BUILD.bazel", repo_cotent)


def _impl_numpy(repository_ctx):

    result = repository_ctx.execute(["python3", "-c", "import numpy as np; print(np.get_include())"])
    prefix = result.stdout.splitlines()[0]
    
    # In Ubuntu 20.04 and python3.6, this points to
    # "/usr/local/lib/python3.6/dist-packages/numpy/core/include"
    repository_ctx.symlink(prefix, "numpy")

    file_content = """
cc_library(
    name = "numpy_cc_library",
    hdrs = glob(["numpy/**/*.h"]),
    strip_include_prefix = "numpy",
    # includes = ["include/numpy"],
    visibility = ["//visibility:public"]
)
    """

    repository_ctx.file("BUILD", file_content)


python_linux = repository_rule(
    implementation=_impl,
    local = True,
    environ = []
)


numpy_linux = repository_rule(
    implementation=_impl_numpy,
    local = True,
    environ = []
)

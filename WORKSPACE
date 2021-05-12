workspace (
    name = "lluvia"
)

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

###########################################################
# download third-party dependencies
###########################################################

load("@lluvia//lluvia/bazel:workspace.bzl", "lluvia_workspace")

lluvia_workspace()


###########################################################
# Python configuration
###########################################################

register_toolchains("@lluvia//lluvia/bazel/python:python_toolchain")

git_repository(
    name = "rules_python",
    remote = "https://github.com/bazelbuild/rules_python.git",
    commit = "6ed1fe53f8b36ecd404d98634d8e7411531cd6f8",
    shallow_since = "1564776078 -0400",
)

load("@rules_python//python:repositories.bzl", "py_repositories")
py_repositories()

# Only needed if using the packaging rules.
load("@rules_python//python:pip.bzl", "pip_repositories", "pip_import")
pip_repositories()

pip_import (
   name = "python_deps",
   requirements = "//:requirements.txt",
   python_interpreter = "python3",
)

load("@python_deps//:requirements.bzl", "pip_install")
pip_install()


# TODO: need to support python 3.5, 3.6, ...
new_local_repository(
    name = "python_linux",
    path = "/usr",
    build_file_content = """
cc_library(
    name = "python3-lib",
    srcs = ["lib/python3.6/config-3.6m-x86_64-linux-gnu/libpython3.6.so"],
    hdrs = glob(["include/python3.6/*.h"]),
    includes = ["include/python3.6"],
    visibility = ["//visibility:public"]
)
    """
)

new_local_repository(
    name = "numpy_linux",
#    path = "/usr/local/lib/python3.6/dist-packages/numpy/core",
    path = "/usr/lib/python3/dist-packages/numpy/core",
    build_file_content = """
cc_library(
    name = "numpy_cc_library",
    hdrs = glob(["include/**/*.h"]),
    strip_include_prefix = "include",
    includes = ["include/numpy"],
    visibility = ["//visibility:public"]
)
    """
)


load("//lluvia/bazel/python:python_windows.bzl", "python_configure", "numpy_configure")
python_configure(name = "python_windows")
numpy_configure(name = "numpy_windows")

# TODO: set path as env variable
new_local_repository(
    name = "vulkan_windows",
    path = "C:/VulkanSDK/1.2.170.0",
    build_file_content = """
cc_library(
    name = "vulkan",
    srcs = ["Lib/vulkan-1.lib"],
    hdrs = glob(["Include/**/*.h"]),
    includes = ["Include"],
    visibility = ["//visibility:public"]
)
    """
)


###########################################################
# Packaging rules
###########################################################

http_archive(
    name = "rules_pkg",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/rules_pkg/releases/download/0.4.0/rules_pkg-0.4.0.tar.gz",
        "https://github.com/bazelbuild/rules_pkg/releases/download/0.4.0/rules_pkg-0.4.0.tar.gz",
    ],
    sha256 = "038f1caa773a7e35b3663865ffb003169c6a71dc995e39bf4815792f385d837d",
)
load("@rules_pkg//:deps.bzl", "rules_pkg_dependencies")
rules_pkg_dependencies()

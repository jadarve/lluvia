workspace (
    name = "lluvia"
)

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive", "http_file")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")



###########################################################
# Python configuration
###########################################################

git_repository(
    name = "rules_python",
    remote = "https://github.com/bazelbuild/rules_python.git",
    commit = "38f86fb55b698c51e8510c807489c9f4e047480e",
    shallow_since = "1575517988 -0500",
)

load("@rules_python//python:repositories.bzl", "py_repositories")
py_repositories()

# Only needed if using the packaging rules.
load("@rules_python//python:pip.bzl", "pip_repositories", "pip3_import")
pip_repositories()


pip3_import (
   name = "python_deps",
   requirements = "//:requirements.txt",
)

load("@python_deps//:requirements.bzl", "pip_install")
pip_install()


###########################################################
# Packaging rules
###########################################################
http_archive(
    name = "rules_pkg",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/rules_pkg/releases/download/0.2.6/rules_pkg-0.2.6.tar.gz",
    ],
    sha256 = "aeca78988341a2ee1ba097641056d168320ecc51372ef7ff8e64b139516a4937",
)
load("@rules_pkg//:deps.bzl", "rules_pkg_dependencies")
rules_pkg_dependencies()


###########################################################
# download third-party dependencies
###########################################################

load("@lluvia//lluvia/bazel:workspace.bzl", "lluvia_workspace")

lluvia_workspace()


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
    path = "/usr/local/lib/python3.6/dist-packages/numpy/core",
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

new_local_repository(
    name = "vulkan_windows",
    path = "C:/VulkanSDK/1.2.135.0",
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


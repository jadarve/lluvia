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

###########################################################
# OS CONFIGURATION
###########################################################

# Linux
load("//lluvia/bazel/python:linux.bzl", "python_linux", "numpy_linux")
python_linux(name = "python_linux")
numpy_linux(name = "numpy_linux")

# Windows
load("//lluvia/bazel/python:windows.bzl", "python_windows", "numpy_windows")
python_windows(name = "python_windows")
numpy_windows(name = "numpy_windows")

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

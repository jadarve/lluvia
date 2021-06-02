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
git_repository(
    name = "rules_python",
    remote = "https://github.com/bazelbuild/rules_python.git",
    commit = "6ed1fe53f8b36ecd404d98634d8e7411531cd6f8",
    shallow_since = "1564776078 -0400",
)

# CONFIGURE: go to platform/values.bzl and change the paths
#            of the python interpreters according to your installation.
#            Only the values for your OS matter.
register_toolchains("@lluvia//platform:python_toolchain")

load("@rules_python//python:repositories.bzl", "py_repositories")
py_repositories()

# Only needed if using the packaging rules.
load("@rules_python//python:pip.bzl", "pip_repositories", "pip_import")
pip_repositories()

# pip_import (
#    name = "python_deps",
#    requirements = "//:requirements.txt",
#    python_interpreter = "python3",
# )

# load("@python_deps//:requirements.bzl", "pip_install")
# pip_install()

###########################################################
# PLATFORM CONFIGURATION
###########################################################

# TODO: Room for improvement here. I should be able to have a single
#       version of both python_lib and numpy_lib regardless of the
#       operating system.

# Linux
load("//platform/linux:python.bzl", "python_linux", "numpy_linux")
python_linux(name = "python_linux")
numpy_linux(name = "numpy_linux")

# Windows
load("//platform/windows:python.bzl", "python_windows", "numpy_windows")
python_windows(name = "python_windows")
numpy_windows(name = "numpy_windows")


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

git_repository(
    name = "rules_vulkan",
    remote = "https://github.com/jadarve/rules_vulkan.git",
    tag = "v0.0.1"
)

load("@rules_vulkan//vulkan:repositories.bzl", "vulkan_repositories")
vulkan_repositories()

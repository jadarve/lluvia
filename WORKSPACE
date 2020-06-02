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
load("@rules_python//python:pip.bzl", "pip_repositories")
pip_repositories()


load("@rules_python//python:pip.bzl", "pip3_import")
pip3_import (
   name = "python_deps",
   requirements = "//:requirements.txt",
)

load("@python_deps//:requirements.bzl", "pip_install")
pip_install()


###########################################################
# download third-party dependencies
###########################################################

# # accessible as @nlohmann_json//file
# http_file (
#     name = "nolhmann_json",
#     urls = [
#         "https://github.com/nlohmann/json/releases/download/v3.0.1/json.hpp"
#     ],
#     sha256 = "c9b3591f1bb94e723a0cd7be861733a3a555b234ef132be1e9027a0364118c4c",
# )

http_file (
    name = "cli11",
    urls = [
        "https://github.com/CLIUtils/CLI11/releases/download/v1.8.0/CLI11.hpp"
    ],
    sha256 = "2dfb4fa5171656ca1bfbbddee5131b8fddb1b83884da30643bfd217f57e91f06",
)

http_archive (
    name = "catch",
    urls = [
        "https://github.com/catchorg/Catch2/archive/v2.11.0.tar.gz"
    ],
    sha256 = "b9957af46a04327d80833960ae51cf5e67765fd264389bd1e275294907f1a3e0",
    strip_prefix = "Catch2-2.11.0",
    build_file = "@//external:catch.bzl"
)

http_archive (
    name = "sol",
    urls = [
        "https://github.com/ThePhD/sol2/archive/v3.0.3.tar.gz"
    ],
    sha256 = "bf089e50387edfc70063e24fd7fbb693cceba4a50147d864fabedd1b33483582",
    strip_prefix = "sol2-3.0.3",
    build_file = "@//external:sol.bzl"
)

new_git_repository(
    name = "stb",
    remote = "https://github.com/nothings/stb.git",
    commit = "f54acd4e13430c5122cab4ca657705c84aa61b08",
    shallow_since = "1580905940 -0800",
    build_file = "@//external:stb.bzl"
)

http_archive (
    name = "lua",
    urls = [
        "https://www.lua.org/ftp/lua-5.3.5.tar.gz"
    ],
    sha256 = "0c2eed3f960446e1a3e4b9a1ca2f3ff893b6ce41942cf54d5dd59ab4b3b058ac",
    strip_prefix = "lua-5.3.5",
    build_file = "@//external:lua.bzl",
)

http_archive(
    name = "cython",
    url = "https://github.com/cython/cython/archive/3.0a1.tar.gz",
    sha256 = "afd96c9113fc334ca14adea53900fa9e28d70a45b44a39e950825f85aed39b04",
    strip_prefix = "cython-3.0a1",
    build_file = "@//external:cython.bzl",
)


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


load("//:bazel/python/python_windows.bzl", "python_configure")
load("//:bazel/python/python_windows.bzl", "numpy_configure")
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


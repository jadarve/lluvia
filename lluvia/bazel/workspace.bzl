"""
"""

load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "new_git_repository")

def lluvia_workspace():
    """Configure the workspace of an external project to work with the rules defined by Lluvia.
    """

#     http_file (
#     name = "cli11",
#     urls = [
#         "https://github.com/CLIUtils/CLI11/releases/download/v1.8.0/CLI11.hpp"
#     ],
#     sha256 = "2dfb4fa5171656ca1bfbbddee5131b8fddb1b83884da30643bfd217f57e91f06",
# )

    maybe (
        repo_rule = http_archive,
        name = "catch",
        urls = [
            "https://github.com/catchorg/Catch2/archive/v2.11.0.tar.gz"
        ],
        sha256 = "b9957af46a04327d80833960ae51cf5e67765fd264389bd1e275294907f1a3e0",
        strip_prefix = "Catch2-2.11.0",
        build_file = "catch.bzl"
    )

    maybe (
        repo_rule = http_archive,
        name = "vulkan",
        urls = [
            "https://github.com/KhronosGroup/Vulkan-Headers/archive/v1.1.130.zip",
        ],
        sha256 = "968e5f6670ea883222c93ef6be176f2310fe0ec51e3be84e3e65542ce45cc348",
        strip_prefix = "Vulkan-Headers-1.1.130",
        build_file = "@lluvia//:external/vulkan.bzl"
    )

    maybe (
        repo_rule = http_archive,
        name = "sol",
        urls = [
            "https://github.com/ThePhD/sol2/archive/v3.0.3.tar.gz"
        ],
        sha256 = "bf089e50387edfc70063e24fd7fbb693cceba4a50147d864fabedd1b33483582",
        strip_prefix = "sol2-3.0.3",
        build_file = "@lluvia//:external/sol.bzl"
    )

    maybe (
        repo_rule = http_archive,
        name = "lua",
        urls = [
            "https://www.lua.org/ftp/lua-5.3.5.tar.gz"
        ],
        sha256 = "0c2eed3f960446e1a3e4b9a1ca2f3ff893b6ce41942cf54d5dd59ab4b3b058ac",
        strip_prefix = "lua-5.3.5",
        build_file = "@lluvia//:external/lua.bzl",
    )

    maybe (
        repo_rule = http_archive,
        name = "miniz",
        urls = [
            "https://github.com/richgel999/miniz/releases/download/2.1.0/miniz-2.1.0.zip"
        ],
        sha256 = "d133132721ad5efbcda2507699d44c54b0da5e31379e4ff049d78d6b1a571f0d",
        build_file = "@lluvia//:external/miniz.bzl"
    )

    maybe (
        repo_rule = new_git_repository,
        name = "stb",
        remote = "https://github.com/nothings/stb.git",
        commit = "f54acd4e13430c5122cab4ca657705c84aa61b08",
        shallow_since = "1580905940 -0800",
        build_file = "stb.bzl"
    )

    maybe (
        repo_rule = http_archive,
        name = "cython",
        url = "https://github.com/cython/cython/archive/3.0a1.tar.gz",
        sha256 = "afd96c9113fc334ca14adea53900fa9e28d70a45b44a39e950825f85aed39b04",
        strip_prefix = "cython-3.0a1",
        build_file = "cython.bzl",
    )

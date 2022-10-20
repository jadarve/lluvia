"""
"""

load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "new_git_repository", "git_repository")

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
            "https://github.com/catchorg/Catch2/archive/v2.13.9.tar.gz"
        ],
        sha256 = "06dbc7620e3b96c2b69d57bf337028bf245a211b3cddb843835bfe258f427a52",
        strip_prefix = "Catch2-2.13.9",
        build_file = "catch.bzl"
    )

    maybe (
        repo_rule = http_archive,
        name = "sol",
        urls = [
            "https://github.com/ThePhD/sol2/archive/v3.2.2.tar.gz"
        ],
        sha256 = "141790dae0c1821dd2dbac3595433de49ba72545845efc3ec7d88de8b0a3b2da",
        strip_prefix = "sol2-3.2.2",
        build_file = "@lluvia//:external/sol.bzl"
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
        url = "https://github.com/cython/cython/releases/download/0.29.28/Cython-0.29.28.tar.gz",
        sha256 = "d6fac2342802c30e51426828fe084ff4deb1b3387367cf98976bb2e64b6f8e45",
        strip_prefix = "Cython-0.29.28",
        build_file = "cython.bzl",
    )

    maybe (
        repo_rule = http_archive,
        name = "rules_python",
        sha256 = "a30abdfc7126d497a7698c29c46ea9901c6392d6ed315171a6df5ce433aa4502",
        strip_prefix = "rules_python-0.6.0",
        url = "https://github.com/bazelbuild/rules_python/archive/0.6.0.tar.gz",
    )

    maybe (
        repo_rule = http_archive,
        name = "ll_rules_pkg",
        urls = [
            "https://mirror.bazel.build/github.com/bazelbuild/rules_pkg/releases/download/0.7.1/rules_pkg-0.7.1.tar.gz",
            "https://github.com/bazelbuild/rules_pkg/releases/download/0.7.1/rules_pkg-0.7.1.tar.gz",
        ],
        sha256 = "451e08a4d78988c06fa3f9306ec813b836b1d076d0f055595444ba4ff22b867f",
    )

    maybe (
        repo_rule = git_repository,
        name = "rules_vulkan",
        remote = "https://github.com/jadarve/rules_vulkan.git",
        commit = "5843b894e83687a67d7eed03fc46309e88d44b6d",
        shallow_since = "1666240744 -0500",
    )

    maybe (
        repo_rule = git_repository,
        name = "rules_lua",
        remote = "https://github.com/jadarve/rules_lua.git",
        commit = "ea7467150e7eec940ba8b9f65b3d399d5078bd6e",
        shallow_since = "1652921905 -0500",
    )

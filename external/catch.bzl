load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "catch_cc_library",
    hdrs = ["single_include/catch2/catch.hpp"],
    strip_include_prefix = "single_include/",
    copts = [
        "--std=c++17",
    ],
    visibility = ["//visibility:public"],
)

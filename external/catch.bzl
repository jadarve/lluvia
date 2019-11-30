load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "catch_cc_library",
    hdrs = ["Catch2-2.11.0/single_include/catch2/catch.hpp"],
    strip_include_prefix = "Catch2-2.11.0/single_include/",
    copts = [
        "--std=c++17",
    ],
    visibility = ["//visibility:public"],
)

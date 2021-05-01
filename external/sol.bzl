load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "sol_cc_library",
    hdrs = glob(["single/include/sol/*.hpp"]),
    strip_include_prefix = "single/include/",
    copts = ({
        "@lluvia//lluvia:linux": [
            "--std=c++17",
        ],
        "@lluvia//lluvia:windows": [
            "/std:c++17",
            "/w",
        ],
    }),
    deps = [
        "@lua//:lua_cc_library",
    ],
    visibility = ["//visibility:public"],
)

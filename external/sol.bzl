load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "sol_cc_library",
    hdrs = glob(["sol2-3.0.3/single/include/sol/*.hpp"]),
    strip_include_prefix = "sol2-3.0.3/single/include/",
    copts = [
        "--std=c++17",
    ],
    deps = [
        "@lua//:lua_cc_library",
    ],
    visibility = ["//visibility:public"],
)

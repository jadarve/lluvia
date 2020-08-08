"""
"""

load("@rules_cc//cc:defs.bzl", "cc_library")

config_setting(
    name = "on_linux",
    constraint_values = [
        "@platforms//os:linux",
    ],
)

config_setting(
    name = "on_windows",
    constraint_values = [
        "@platforms//os:windows",
    ],
)

cc_library(
    name = "miniz_cc_library",
    hdrs = [
        "miniz.h"
    ],
    srcs = [
        "miniz.c"
    ],
    linkstatic = True,
    visibility = ["//visibility:public"],
)

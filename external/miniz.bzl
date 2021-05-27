"""
"""

load("@rules_cc//cc:defs.bzl", "cc_library")

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

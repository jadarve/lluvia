load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "stb_image_cc_library",
    hdrs = [
        "stb_image.h",
        "stb_image_write.h",
    ],
    copts = [
        "--std=c++17",
    ],
    visibility = ["//visibility:public"],
)

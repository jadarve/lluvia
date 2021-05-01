load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "stb_image_cc_library",
    hdrs = [
        "stb_image.h",
        "stb_image_write.h",
    ],
    copts = select({
        "@lluvia//lluvia:linux": [
            "--std=c++17"
        ],
        "@lluvia//lluvia:windows": [
            "/std:c++17"
        ],
        "//conditions:default": [
            "--std=c++17"
        ]
    })
    visibility = ["//visibility:public"],
)

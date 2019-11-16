"""
"""

load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library (
    name = "nolhmann_json_library",
    srcs = [
        "@nolhmann_json//file"
    ],
    hdrs = [
        "@nolhmann_json//file"
    ]
)
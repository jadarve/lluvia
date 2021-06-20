"""
"""

CC_TEST_COPTS = select({
    "@lluvia//:linux": [
      "-std=c++17",
    ],
    "@lluvia//:windows": [
      "/std:c++17",
      "/w",
    ],
})

CC_TEST_DEPS = [
    "@lluvia//lluvia/cpp/core:core_cc_library",
    "@catch//:catch_cc_library",
    "@bazel_tools//tools/cpp/runfiles:runfiles",
]

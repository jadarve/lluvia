load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "vulkan_cc_library",
    hdrs = glob(["include/vulkan/*.hpp",
                 "include/vulkan/*.h"]),
    strip_include_prefix = "include/",
    visibility = ["//visibility:public"],
)

load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_binary (
    name = "moni",
    srcs = [
        "main.cpp",
    ],
    deps = [
        ":moni_cc_library",
        "@sol//:sol_cc_library",
        "@lua//:lua_cc_library",
        # "//:cli11_cc_library"
    ],
    copts = [
        "--std=c++17",
        "-stdlib=libstdc++",
    ],
    linkstatic = True,
    linkopts = [
        "-lm",
        "-ldl",
    ],
    visibility = ["//visibility:public"]
)


cc_library (
    name = "moni_cc_library",
    srcs = [
        "src/moni/moni.cpp"
    ],
    hdrs = [
        "include/moni/moni.h"
    ],
    strip_include_prefix = "include/",
    visibility = ["//visibility:public"]
)
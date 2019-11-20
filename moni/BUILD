# load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary (
    name = "moni",
    srcs = [
        "main.cpp",
    ],
    deps = [
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

"""
"""

load("@rules_cc//cc:defs.bzl", "cc_library", "cc_binary")

cc_library (
    name = "lua_cc_library",
    srcs = glob(
        ["src/*.c", "src/*.h", "src/*.hpp"],
        exclude = ["src/lua.c", "src/luac.c"],
    ),
    hdrs = glob(["src/*.h", "src/*.hpp"]),
    strip_include_prefix = "src",
    linkstatic = True,
    local_defines = select({
        "@lluvia//:linux": [
            "LUA_USE_LINUX"
        ],
        "//conditions:default": [
        ]
    }),
    visibility = ["//visibility:public"],
)

cc_binary (
    name = "lua",
    srcs = ["src/lua.c"],
    deps = [
        ":lua_cc_library"
    ],
    linkstatic = True,
    linkopts = select({
        "@lluvia//:linux": [
            "-lm",
            "-ldl"
        ],
        "@lluvia//:android": [
            "-lm",
            "-ldl"
        ],
        "//conditions:default": [
        ]
    }),
    visibility = ["//visibility:public"],
)

cc_binary (
    name = "luac",
    srcs = ["src/luac.c"],
    deps = [
        ":lua_cc_library"
    ],
    linkstatic = True,
    linkopts = select({
        "@lluvia//:linux": [
            "-lm",
            "-ldl"
        ],
        "@lluvia//:android": [
            "-lm",
            "-ldl"
        ],
        "//conditions:default": [

        ]
    }),
    visibility = ["//visibility:public"],
)

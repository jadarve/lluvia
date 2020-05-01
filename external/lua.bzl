"""
"""

load("@rules_cc//cc:defs.bzl", "cc_library", "cc_binary")

cc_library (
    name = "lua_cc_library",
    srcs = glob(
        ["src/*.c"],
        exclude = ["src/lua.c", "src/luac.c"],
    ),
    hdrs = glob(["src/*.h", "src/*.hpp"]),
    strip_include_prefix = "src",
    linkstatic = True,
    local_defines = [
        "LUA_USE_LINUX",
    ],
    visibility = ["//visibility:public"],
)

cc_binary (
    name = "lua",
    srcs = ["src/lua.c"],
    deps = [
        ":lua_cc_library"
    ],
    linkstatic = True,
    linkopts = [
        "-lm",
        "-ldl"
    ],
    visibility = ["//visibility:public"],
)

cc_binary (
    name = "luac",
    srcs = ["src/luac.c"],
    deps = [
        ":lua_cc_library"
    ],
    linkstatic = True,
    linkopts = [
        "-lm",
        "-ldl"
    ],
    visibility = ["//visibility:public"],
)

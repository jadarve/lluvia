"""
"""

load("@rules_cc//cc:defs.bzl", "cc_library", "cc_binary")

cc_library (
    name = "lua_cc_library",
    srcs = glob(
        ["lua-5.3.5/src/*.c"],
        exclude = ["lua-5.3.5/src/lua.c", "lua-5.3.5/src/luac.c"],
    ),
    hdrs = glob(["lua-5.3.5/src/*.h", "lua-5.3.5/src/*.hpp"]),
    strip_include_prefix = "lua-5.3.5/src",
    linkstatic = True,
    local_defines = [
        "LUA_USE_LINUX",
    ],
    visibility = ["//visibility:public"],
)

cc_binary (
    name = "lua",
    srcs = ["lua-5.3.5/src/lua.c"],
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
    srcs = ["lua-5.3.5/src/luac.c"],
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

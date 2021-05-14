"""
"""

def _impl(repository_ctx):

    sdk_path = repository_ctx.os.environ["VULKAN_SDK"]

    repository_ctx.symlink(sdk_path, "vulkan_sdk_windows")

    glslc_program = repository_ctx.which("glslc.exe")
    print("GLSLC_PROGRAM: {0}".format(glslc_program))

    file_content = """
cc_library(
    name = "vulkan",
    srcs = ["vulkan_sdk_windows/Lib/vulkan-1.lib"],
    hdrs = glob(["vulkan_sdk_windows/Include/**/*.h"]),
    includes = ["vulkan_sdk_windows/Include"],
    visibility = ["//visibility:public"]
)

"""
    glslc_file_content = """

glslc = "<GLSLC_PROGRAM>"

""".replace("<GLSLC_PROGRAM>", str(glslc_program))

    repository_ctx.file("BUILD.bazel", file_content)
    repository_ctx.file("glslc.bzl", glslc_file_content)

vulkan_windows = repository_rule(
    implementation=_impl,
    local = True,
    environ = ["VULKAN_SDK"]
)

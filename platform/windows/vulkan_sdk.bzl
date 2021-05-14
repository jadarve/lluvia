"""
"""

def _impl(repository_ctx):

    sdk_path = repository_ctx.os.environ["VULKAN_SDK"]

    repository_ctx.symlink(sdk_path, "vulkan_sdk_windows")

    file_content = """
cc_library(
    name = "vulkan",
    srcs = ["vulkan_sdk_windows/Lib/vulkan-1.lib"],
    hdrs = glob(["vulkan_sdk_windows/Include/**/*.h"]),
    includes = ["vulkan_sdk_windows/Include"],
    visibility = ["//visibility:public"]
)
"""

    repository_ctx.file("BUILD", file_content)

vulkan_windows = repository_rule(
    implementation=_impl,
    local = True,
    environ = ["VULKAN_SDK"]
)

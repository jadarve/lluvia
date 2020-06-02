def _impl(repository_ctx):
  result = repository_ctx.execute(["python", "-c", "from distutils.sysconfig import get_config_var; print(get_config_var('prefix'))"])
  prefix = result.stdout.splitlines()[0]
  repository_ctx.symlink(prefix, "python")
  file_content = """
cc_library(
    name = "python3-lib",
    srcs = ["python/libs/python38.lib"],
    hdrs = glob(["python/include/*.h"]),
    includes = ["python/include"],
    visibility = ["//visibility:public"]
)
"""
  repository_ctx.file("BUILD", file_content)

python_configure = repository_rule(
    implementation=_impl,
    local = True,
    environ = []
)


def _impl_numpy(repository_ctx):
  result = repository_ctx.execute(["python", "-c", "from distutils.sysconfig import get_config_var; print(get_config_var('LIBDEST'))"])
  prefix = result.stdout.splitlines()[0] + "/site-packages/numpy/core"
  repository_ctx.symlink(prefix, "numpy")
  file_content = """
cc_library(
    name = "numpy_cc_library",
    hdrs = glob(["numpy/include/**/*.h"]),
    strip_include_prefix = "numpy/include",
    includes = ["numpy/include/numpy"],
    visibility = ["//visibility:public"]
)
"""
  repository_ctx.file("BUILD", file_content)

numpy_configure = repository_rule(
    implementation=_impl_numpy,
    local = True,
    environ = []
)

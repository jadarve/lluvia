workspace (
    name = "lluvia"
)

###########################################################
# download third-party dependencies
###########################################################

load("@lluvia//lluvia/bazel:workspace.bzl", "lluvia_workspace")
lluvia_workspace()


###########################################################
# Python configuration
###########################################################
# CONFIGURE: go to platform/values.bzl and change the paths
#            of the python interpreters according to your installation.
#            Only the values for your OS matter.
register_toolchains("@lluvia//platform:python_toolchain")


# Only needed if using the packaging rules.
load("@rules_python//python:pip.bzl", "pip_repositories")
pip_repositories()


###########################################################
# PLATFORM CONFIGURATION
###########################################################

# TODO: Room for improvement here. I should be able to have a single
#       version of both python_lib and numpy_lib regardless of the
#       operating system.

# Linux
load("@lluvia//platform/linux:python.bzl", "python_linux", "numpy_linux")
python_linux(name = "python_linux")
numpy_linux(name = "numpy_linux")

# Windows
load("@lluvia//platform/windows:python.bzl", "python_windows", "numpy_windows")
python_windows(name = "python_windows")
numpy_windows(name = "numpy_windows")


###########################################################
# Packaging rules
###########################################################
load("@ll_rules_pkg//:deps.bzl", "rules_pkg_dependencies")
rules_pkg_dependencies()

###########################################################
# Vulkan rules
###########################################################
load("@rules_vulkan//vulkan:repositories.bzl", "vulkan_repositories")
vulkan_repositories(
    android_use_host_vulkan_sdk = True
)


###########################################################
# Lua rules
###########################################################
load("@rules_lua//toolchains:repositories.bzl", "lua_repositories")
lua_repositories()

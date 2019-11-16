workspace (
    name = "lluvia"
)

###########################################################
# download third-party dependencies
###########################################################
# include(cmake/thirdparty/Catch.cmake)
# include(cmake/thirdparty/JSON.cmake)
# include(cmake/thirdparty/CLI.cmake)
# include(cmake/thirdparty/STB.cmake)
# include(cmake/thirdparty/Lua.cmake)
# include(cmake/thirdparty/Sol2.cmake)

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive", "http_file")

# accessible as @nlohmann_json//file
http_file (
    name = "nolhmann_json",
    urls = [
        "https://github.com/nlohmann/json/releases/download/v3.0.1/json.hpp"
    ],
    sha256 = "c9b3591f1bb94e723a0cd7be861733a3a555b234ef132be1e9027a0364118c4c"
)

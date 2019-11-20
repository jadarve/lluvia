workspace (
    name = "lluvia"
)

###########################################################
# download third-party dependencies
###########################################################
load("@bazel_tools//tools/build_defs/repo:http.bzl",
    "http_archive", "http_file")

# accessible as @nlohmann_json//file
http_file (
    name = "nolhmann_json",
    urls = [
        "https://github.com/nlohmann/json/releases/download/v3.0.1/json.hpp"
    ],
    sha256 = "c9b3591f1bb94e723a0cd7be861733a3a555b234ef132be1e9027a0364118c4c",
)

http_file (
    name = "cli11",
    urls = [
        "https://github.com/CLIUtils/CLI11/releases/download/v1.8.0/CLI11.hpp"
    ],
    sha256 = "2dfb4fa5171656ca1bfbbddee5131b8fddb1b83884da30643bfd217f57e91f06",
)

http_file (
    name = "catch",
    urls = [
        "https://github.com/catchorg/Catch2/releases/download/v2.0.1/catch.hpp"
    ],
    sha256 = "6988c82def026a0b862f43ab122bbbd6051b0b7648bfe04c8a150ef62ca5a0a2"
)

http_archive (
    name = "sol",
    urls = [
        "https://github.com/ThePhD/sol2/archive/v3.0.3.tar.gz"
    ],
    sha256 = "bf089e50387edfc70063e24fd7fbb693cceba4a50147d864fabedd1b33483582",
    build_file = "@//external:sol.bzl"
)

http_file (
    name = "stb_image",
    urls = [
        "https://raw.githubusercontent.com/nothings/stb/master/stb_image.h"
    ],
    # sha256 = "0e28238d865510073b5740ae8eba8cd8032cc5b25f94e0f7505fac8036864909",
)

http_file (
    name = "stb_image_write",
    urls = [
        "https://raw.githubusercontent.com/nothings/stb/master/stb_image_write.h"
    ],
    # sha256 = "0e8b3d80bc6eb8fdb64abc4db9fec608b489bc73418eaf14beda102a0699a4c9",
)

http_archive (
    name = "lua",
    urls = [
        "https://www.lua.org/ftp/lua-5.3.5.tar.gz"
    ],
    sha256 = "0c2eed3f960446e1a3e4b9a1ca2f3ff893b6ce41942cf54d5dd59ab4b3b058ac",
    build_file = "@//external:lua.bzl",
)

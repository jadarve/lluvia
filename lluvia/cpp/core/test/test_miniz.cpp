#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#if defined(__GNUC__)
// Ensure we get the 64-bit variants of the CRT's file I/O calls
#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE 1
#endif
#endif

#include "miniz.h"

#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

#include "tools/cpp/runfiles/runfiles.h"
using bazel::tools::cpp::runfiles::Runfiles;

std::vector<uint8_t> readFile(const std::string& path, std::vector<uint8_t>& out) {

    // auto out = std::vector<uint8_t>();

    std::ifstream file{ path, std::ios::ate | std::ios::binary };
    file.exceptions(std::ifstream::badbit | std::ifstream::failbit);

    const auto fileSize = static_cast<size_t>(file.tellg());
    out.resize(fileSize);

    file.seekg(0);
    file.read(reinterpret_cast<char *>(out.data()), fileSize);
    file.close();

    return out;
}


TEST_CASE("ComparedContent", "test_miniz") {

    auto runfiles = Runfiles::CreateForTest(nullptr);
    REQUIRE(runfiles != nullptr);

    // read a lua file
    // read a spirv file
    // read a .zip file containing both (a ll_node_library target)
    // compare binary data of both

    // read lua and spirv files
    auto lua_file = std::vector<uint8_t> {};
    auto spirv_file = std::vector<uint8_t> {};
    REQUIRE_NOTHROW(readFile(runfiles->Rlocation("lluvia/lluvia/cpp/core/test/nodes/Assign.lua"), lua_file));
    REQUIRE_NOTHROW(readFile(runfiles->Rlocation("lluvia/lluvia/cpp/core/test/nodes/Assign.comp.spv"), spirv_file));

    // read zip archive
    auto zip_archive = mz_zip_archive {};

    // Now try to open the archive.
    memset(&zip_archive, 0, sizeof(zip_archive));

    mz_bool status = mz_zip_reader_init_file(&zip_archive, runfiles->Rlocation("lluvia/lluvia/cpp/core/test/nodes/test_node_library.zip").c_str(), 0);
    REQUIRE(status == true);
    
    // Get and print information about each file in the archive.
    for (auto i = 0; i < (int)mz_zip_reader_get_num_files(&zip_archive); i++) {
        mz_zip_archive_file_stat file_stat;
        REQUIRE(mz_zip_reader_file_stat(&zip_archive, i, &file_stat));

        std::cout << "Filename     : " << file_stat.m_filename << std::endl;
        std::cout << "Comment      : " << file_stat.m_comment << std::endl;
        std::cout << "Uncompressed : " << file_stat.m_uncomp_size << std::endl;
        std::cout << "Compressed   : " << file_stat.m_comp_size << std::endl;
        std::cout << "Is dir       : " << file_stat.m_is_directory << std::endl << std::endl;


        std::vector<uint8_t>& raw_data = std::string{file_stat.m_filename} == "nodes/Assign.lua"? lua_file : spirv_file;

        auto uncomp_size = size_t{};
        auto data = static_cast<uint8_t*>(mz_zip_reader_extract_file_to_heap(&zip_archive, file_stat.m_filename, &uncomp_size, 0));

        REQUIRE(uncomp_size == raw_data.size());
        REQUIRE(std::memcmp(data, raw_data.data(), uncomp_size) == 0);
    }

    mz_zip_reader_end(&zip_archive);
}

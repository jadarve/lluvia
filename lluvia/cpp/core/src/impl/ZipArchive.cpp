/**
@file       ZipArchive.cpp
@brief      ZipArchive class.
@copyright  2020, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#include "lluvia/core/impl/ZipArchive.h"

#include "lluvia/core/error.h"


namespace ll {
namespace impl {

ZipArchive::ZipArchive(const std::string& filename) {
    ll::throwSystemErrorIf(mz_zip_reader_init_file(&mArchive, filename.c_str(), 0) == MZ_FALSE, ll::ErrorCode::IOError, "Error reading archive");
}

ZipArchive::~ZipArchive() {
    mz_zip_reader_end(&mArchive);
}


size_t ZipArchive::numberFiles() {
    return static_cast<size_t>(mz_zip_reader_get_num_files(&mArchive));
}


mz_zip_archive_file_stat ZipArchive::getFileStat(const size_t i) {

    ll::throwSystemErrorIf(i >= numberFiles(), ll::ErrorCode::IOError, "File index must be less than the number of files in the archive.");

    auto stat = mz_zip_archive_file_stat{};
    ll::throwSystemErrorIf(!mz_zip_reader_file_stat(&mArchive, i, &stat), ll::ErrorCode::IOError, "Error reading file stats from zip archive");

    return stat;
}

std::string ZipArchive::uncompressTextFile(mz_zip_archive_file_stat& stat) {

    auto buffer = std::vector<char>{};
    buffer.resize(stat.m_uncomp_size);

    ll::throwSystemErrorIf(!mz_zip_reader_extract_file_to_mem(&mArchive, stat.m_filename, &buffer[0], buffer.size(), 0),
        ll::ErrorCode::IOError, "Error extracting text file from archive.");

    return std::string {&buffer[0], buffer.size()};
}

std::vector<uint8_t> ZipArchive::uncompressBinaryFile(mz_zip_archive_file_stat& stat) {

    auto buffer = std::vector<uint8_t>{};
    buffer.resize(stat.m_uncomp_size);

    ll::throwSystemErrorIf(!mz_zip_reader_extract_file_to_mem(&mArchive, stat.m_filename, &buffer[0], buffer.size(), 0),
        ll::ErrorCode::IOError, "Error extracting binary file from archive.");
    
    return buffer;
}

} // namespace impl
} // namespace ll

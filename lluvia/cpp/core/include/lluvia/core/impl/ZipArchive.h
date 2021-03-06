/**
@file       ZipArchive.h
@brief      ZipArchive class.
@copyright  2020, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_IMPL_ZIP_ARCHIVE_H_
#define LLUVIA_CORE_IMPL_ZIP_ARCHIVE_H_

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

#include <string>
#include <vector>

namespace ll {
namespace impl {


class ZipArchive {

public:
    ZipArchive()                               = delete;
    ZipArchive(const ZipArchive&)              = delete;
    ZipArchive(ZipArchive&&)                   = delete;
    ZipArchive(const std::string& filename);

    ~ZipArchive();

    ZipArchive& operator = (const ZipArchive&) = delete;
    ZipArchive& operator = (ZipArchive&&)      = delete;

    size_t numberFiles();

    mz_zip_archive_file_stat getFileStat(const size_t i);

    std::string uncompressTextFile(mz_zip_archive_file_stat& stat);

    std::vector<uint8_t> uncompressBinaryFile(mz_zip_archive_file_stat& stat);


private:
    mz_zip_archive mArchive{};
};

} // namespace impl
} // namespace ll

#endif // LLUVIA_CORE_IMPL_ZIP_ARCHIVE_H_

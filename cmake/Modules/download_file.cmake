#! download_file : download a file and place it in a given file path.
#
# Optionally, this function computes the checksum of the
# file downloaded and compares it against a given value. An error message
# is printed if the checksums do not match.
#
#
# \argn: 4
# \arg:URL of the file to download.
# \arg:PATH file path where the downloaded filed will be stored
# \arg:CHECKSUM_TYPE (optional) algorithm to compute the checksum value.
#   possible values: MD5, SHA1, SHA224, SHA256, SHA384, SHA512
# \arg:CHECKSUM_VALUE (optional) checksum value to compare against.
#
function (download_file URL PATH)

    message (STATUS "Downloading: ${URL} into ${PATH}")

    file (DOWNLOAD ${URL} ${PATH})

    # if the checksum type and value are provided
    if (${ARGC} EQUAL 4)

        file (${ARGV2} ${PATH} _CHECKSUM_RESULT)

        # compare the checksums
        if (NOT ${ARGV3} STREQUAL _CHECKSUM_RESULT)
            message (ERROR " Checksums for ${URL} do not match:
                expected: ${ARGV3}
                got:      ${_CHECKSUM_RESULT}")
        endif()
    endif()

endfunction()

message (STATUS "-------------------------------------------------")
message (STATUS "Configuring STB")

set (_STB_FILE_LIST
    "stb_image.h"
    "stb_image_write.h"
)

set (_STB_FILE_PATH "${CMAKE_BINARY_DIR}/thirdparty/stb")

foreach(_FILE ${_STB_FILE_LIST})

    set (_FPATH "${_STB_FILE_PATH}/${_FILE}")

    if (NOT EXISTS ${_FPATH})

        download_file (
            "https://raw.githubusercontent.com/nothings/stb/master/${_FILE}"
            ${_FPATH}
        )
        
    endif()

endforeach(_FILE)

set (STB_INCLUDE_DIR "${CMAKE_BINARY_DIR}/thirdparty/stb")
add_library (STB INTERFACE)
target_include_directories (STB INTERFACE ${STB_INCLUDE_DIR})

message (STATUS "-------------------------------------------------")

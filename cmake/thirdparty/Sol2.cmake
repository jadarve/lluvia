message (STATUS "-------------------------------------------------")
message (STATUS "Configuring Sol2")

set (_TO_DOWNLOAD
    "sol.hpp"
    "forward.hpp"
)

set(_FILE_PATH "${CMAKE_SOURCE_DIR}/thirdparty/sol")
foreach(_FILE ${_TO_DOWNLOAD})

    set (_FPATH "${_FILE_PATH}/${_FILE}")
    if (NOT EXISTS ${_FPATH})

        download_file (
            "https://github.com/ThePhD/sol2/releases/download/v3.0.2/${_FILE}"
            ${_FPATH}
        )
        
    endif()

endforeach(_FILE)

set (SOL_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/thirdparty/sol")
add_library (sol INTERFACE)
target_include_directories (sol INTERFACE ${SOL_INCLUDE_DIR})

message (STATUS "-------------------------------------------------")

message (STATUS "-------------------------------------------------")
message (STATUS "Configuring JSON")

set (JSON_FILE_PATH "${CMAKE_BINARY_DIR}/thirdparty/json/json.hpp")

# download the file only if it does not exists
if (NOT EXISTS ${JSON_FILE_PATH})
    
    download_file (
        "https://github.com/nlohmann/json/releases/download/v3.0.1/json.hpp"
        ${JSON_FILE_PATH}
        SHA256
        "c9b3591f1bb94e723a0cd7be861733a3a555b234ef132be1e9027a0364118c4c"
    )

endif()

set (JSON_INCLUDE_DIR "${CMAKE_BINARY_DIR}/thirdparty")
set (JSON_INCLUDE_DIR ${JSON_INCLUDE_DIR} CACHE INTERNAL "JSON include directory")

add_library (json INTERFACE)
target_include_directories (json INTERFACE ${JSON_INCLUDE_DIR})

message (STATUS "JSON_INCLUDE_DIR: ${JSON_INCLUDE_DIR}")
message (STATUS "-------------------------------------------------")

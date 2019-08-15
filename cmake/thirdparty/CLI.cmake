message (STATUS "-------------------------------------------------")
message (STATUS "Configuring CLI11")

set (CLI_FILE_PATH "${CMAKE_SOURCE_DIR}/thirdparty/CLI11/CLI11.hpp")

# download the file only if it does not exists
if (NOT EXISTS ${CLI_FILE_PATH})
    
    download_file (
        "https://github.com/CLIUtils/CLI11/releases/download/v1.8.0/CLI11.hpp"
        ${CLI_FILE_PATH}
    )

endif()

set (CLI_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/thirdparty")
set (CLI_INCLUDE_DIR ${CLI_INCLUDE_DIR} CACHE INTERNAL "JSON include directory")

add_library (cli11 INTERFACE)
target_include_directories (cli11 INTERFACE ${CLI_INCLUDE_DIR})

message (STATUS "CLI_INCLUDE_DIR: ${CLI_INCLUDE_DIR}")
message (STATUS "-------------------------------------------------")

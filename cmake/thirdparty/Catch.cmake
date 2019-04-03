message (STATUS "-------------------------------------------------")
message (STATUS "Configuring Catch")

set (CATCH_FILE_PATH "${CMAKE_SOURCE_DIR}/thirdparty/catch/catch.hpp")

# download the file only if it does not exists
if (NOT EXISTS ${CATCH_FILE_PATH})

    download_file (
        "https://github.com/catchorg/Catch2/releases/download/v2.0.1/catch.hpp"
        ${CATCH_FILE_PATH}
    )
    
endif()

set (CATCH_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/thirdparty")
add_library (Catch INTERFACE)
target_include_directories (Catch INTERFACE ${CATCH_INCLUDE_DIR})

message (STATUS "-------------------------------------------------")

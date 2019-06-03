message (STATUS "-------------------------------------------------")
message (STATUS "Configuring ChaiScript")

set (CHAISCRIPT_VERSION "6.1.0")
set (CHAISCRIPT_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/thirdparty/ChaiScript-${CHAISCRIPT_VERSION}/include")

if (NOT EXISTS ${CHAISCRIPT_INCLUDE_DIR})

    set (CHAISCRIPT_ZIP_PATH "${CMAKE_SOURCE_DIR}/thirdparty/chaiscript.zip")
    download_file (
        "https://github.com/ChaiScript/ChaiScript/archive/v${CHAISCRIPT_VERSION}.zip"
        ${CHAISCRIPT_ZIP_PATH}
    )

    # unzip and remove .zip file
    execute_process(COMMAND unzip ${CHAISCRIPT_ZIP_PATH} -d ${CMAKE_SOURCE_DIR}/thirdparty)
    execute_process(COMMAND rm ${CHAISCRIPT_ZIP_PATH})

endif()

add_library (ChaiScript INTERFACE)
target_include_directories (ChaiScript INTERFACE ${CHAISCRIPT_INCLUDE_DIR})

message (STATUS "-------------------------------------------------")

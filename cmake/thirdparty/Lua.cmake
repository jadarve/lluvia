message (STATUS "-------------------------------------------------")
message (STATUS "Configuring Lua")

set (LUA_VERSION "5.3.5")
set (LUA_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/thirdparty/lua-${LUA_VERSION}/src")

if (NOT EXISTS ${LUA_INCLUDE_DIR})

    set (LUA_ZIP_PATH "${CMAKE_SOURCE_DIR}/thirdparty/lua.tar.gz")
    download_file (
        "https://www.lua.org/ftp/lua-${LUA_VERSION}.tar.gz"
        ${LUA_ZIP_PATH}
        SHA1
        112eb10ff04d1b4c9898e121d6bdf54a81482447
    )

    # untar and remove tar file
    execute_process(COMMAND tar -xvf ${LUA_ZIP_PATH}
                    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/thirdparty")
    execute_process(COMMAND rm ${LUA_ZIP_PATH})

endif()

# manually compile lua library
add_library (lua STATIC
    ${LUA_INCLUDE_DIR}/lapi.c
    ${LUA_INCLUDE_DIR}/lcode.c
    ${LUA_INCLUDE_DIR}/lctype.c
    ${LUA_INCLUDE_DIR}/ldebug.c
    ${LUA_INCLUDE_DIR}/ldo.c
    ${LUA_INCLUDE_DIR}/ldump.c
    ${LUA_INCLUDE_DIR}/lfunc.c
    ${LUA_INCLUDE_DIR}/lgc.c
    ${LUA_INCLUDE_DIR}/llex.c
    ${LUA_INCLUDE_DIR}/lmem.c
    ${LUA_INCLUDE_DIR}/lobject.c
    ${LUA_INCLUDE_DIR}/lopcodes.c
    ${LUA_INCLUDE_DIR}/lparser.c
    ${LUA_INCLUDE_DIR}/lstate.c
    ${LUA_INCLUDE_DIR}/lstring.c
    ${LUA_INCLUDE_DIR}/ltable.c
    ${LUA_INCLUDE_DIR}/ltm.c
    ${LUA_INCLUDE_DIR}/lundump.c
    ${LUA_INCLUDE_DIR}/lvm.c
    ${LUA_INCLUDE_DIR}/lzio.c
    ${LUA_INCLUDE_DIR}/lauxlib.c
    ${LUA_INCLUDE_DIR}/lbaselib.c
    ${LUA_INCLUDE_DIR}/lbitlib.c
    ${LUA_INCLUDE_DIR}/lcorolib.c
    ${LUA_INCLUDE_DIR}/ldblib.c
    ${LUA_INCLUDE_DIR}/liolib.c
    ${LUA_INCLUDE_DIR}/lmathlib.c
    ${LUA_INCLUDE_DIR}/loslib.c
    ${LUA_INCLUDE_DIR}/lstrlib.c
    ${LUA_INCLUDE_DIR}/ltablib.c
    ${LUA_INCLUDE_DIR}/lutf8lib.c
    ${LUA_INCLUDE_DIR}/loadlib.c
    ${LUA_INCLUDE_DIR}/linit.c
)

target_compile_definitions(lua
    PUBLIC
    LUA_USE_LINUX
)

target_link_libraries(lua
    m
    dl
)

target_include_directories(lua
    INTERFACE
    ${LUA_INCLUDE_DIR}
)

message (STATUS "-------------------------------------------------")

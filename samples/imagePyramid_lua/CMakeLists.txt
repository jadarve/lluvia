message (STATUS "-------------------------------------------------")
message (STATUS "Building imagePyramid")

add_executable(imagePyramid_lua
    src/main.cpp
)


set (_SHADER_LIST
    glsl/ImageDownsampleX.comp
    glsl/ImageDownsampleY.comp
)

set (_SHADERS_OUTPUT_LIST)
foreach (_SHADER ${_SHADER_LIST})

    set (_SHADER_TARGET "_SHADER_TARGET")
    llAddShader("${CMAKE_CURRENT_SOURCE_DIR}/${_SHADER}" ${CMAKE_CURRENT_BINARY_DIR} ${_SHADER_TARGET})
    list (APPEND _SHADERS_OUTPUT_LIST ${_SHADER_TARGET})

endforeach ()


set (_LUA_LIST
    ImageDownsampleX.lua
    ImageDownsampleY.lua
)
foreach(_LUA ${_LUA_LIST})
    file(COPY lua/${_LUA} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/lua)
endforeach()

add_custom_target (imagePyramidShaders_lua ALL DEPENDS ${_SHADERS_OUTPUT_LIST})
add_dependencies (imagePyramid_lua imagePyramidShaders_lua)


target_include_directories(imagePyramid_lua
    PUBLIC ${LL_INCLUDE_DIRS}
)

target_link_libraries(imagePyramid_lua
    ${LL_LINK_LIBRARIES}
    cli11
    m
    STB
)

message (STATUS "-------------------------------------------------")

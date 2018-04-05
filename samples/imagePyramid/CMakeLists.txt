message (STATUS "-------------------------------------------------")
message (STATUS "Building imagePyramid")

add_executable(imagePyramid
    src/ImagePyramid.cpp
    src/main.cpp
)


set (_SHADER_LIST
    glsl/imageDownsampleX.comp
    glsl/imageDownsampleY.comp
)

set (_SHADERS_OUTPUT_LIST)
foreach (_SHADER ${_SHADER_LIST})

    set (_SHADER_TARGET "_SHADER_TARGET")
    llAddShader("${CMAKE_CURRENT_SOURCE_DIR}/${_SHADER}" ${CMAKE_CURRENT_BINARY_DIR} ${_SHADER_TARGET})
    list (APPEND _SHADERS_OUTPUT_LIST ${_SHADER_TARGET})

endforeach ()

add_custom_target (imagePyramidShaders ALL DEPENDS ${_SHADERS_OUTPUT_LIST})
add_dependencies (imagePyramid imagePyramidShaders)

# message (STATUS "ImagePyramid: LL_INCLUDE_DIRS: ${LL_INCLUDE_DIRS}")
# message (STATUS "ImagePyramid: LL_LINK_LIBRARIES: ${LL_LINK_LIBRARIES}")

target_include_directories(imagePyramid
    PUBLIC ${LL_INCLUDE_DIRS}
    PUBLIC include
)

target_link_libraries(imagePyramid
    ${LL_LINK_LIBRARIES}
    m
    STB
)

message (STATUS "-------------------------------------------------")

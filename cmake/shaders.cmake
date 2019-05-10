
set (_SHADERS_OUTPUT_LIST)

foreach (_SRC ${LL_GLSL_SRCS})

    set (_GLSL_OUTPUT "_GLSL_OUTPUT")
    llAddShader(${_SRC} ${LL_GLSL_OUTPUT_DIR} ${_GLSL_OUTPUT})

    list (APPEND _SHADERS_OUTPUT_LIST ${_GLSL_OUTPUT})

endforeach ()

message (STATUS "shaders.cmake: ${_SHADERS_OUTPUT_LIST}")

# shaders target depends on all the shader commands
add_custom_target (shaders ALL
    DEPENDS ${_SHADERS_OUTPUT_LIST}
)

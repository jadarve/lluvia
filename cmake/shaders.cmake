set (LLC_COMMAND ${CMAKE_BINARY_DIR}/tools/llc/llc)
message (STATUS "LLC_COMMAND: ${LLC_COMMAND}")


set (_SHADERS_OUTPUT_LIST)

foreach (_SRC ${LL_GLSL_SRCS})
    
    get_filename_component(_GLSL_NAME_WE ${_SRC} NAME_WE)

    set (_GLSL_OUTPUT ${LL_SHADER_OUTPUT_DIR}/${_GLSL_NAME_WE}.spv)

    add_custom_command (
        OUTPUT ${_GLSL_OUTPUT}
        COMMAND ${LLC_COMMAND} -o ${_GLSL_OUTPUT} ${_SRC}
        MAIN_DEPENDENCY ${_SRC}
        COMMENT "Building compute shader ${_GLSL_OUTPUT}"
    )

    list (APPEND _SHADERS_OUTPUT_LIST ${_GLSL_OUTPUT})

endforeach ()

# shaders target depends on all the shader commands
add_custom_target (shaders ALL
    DEPENDS ${_SHADERS_OUTPUT_LIST}
)

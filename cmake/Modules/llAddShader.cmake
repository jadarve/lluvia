#! llAddShader : add a shader target
#
# \argn: 3
# \arg:SRC         path to the source file
# \arg:OUT_PATH    folder path where the compiled shader will be written.
# \arg:TARGET_NAME name of the CMake variable where output target will be stored.
#
function (llAddShader SRC OUT_PATH TARGET_NAME)

    message (STATUS "llAddShader: ${SRC}")

    get_filename_component(_GLSL_NAME_WE ${SRC} NAME_WE)
    set (_GLSL_OUTPUT ${OUT_PATH}/${_GLSL_NAME_WE}.spv)

    add_custom_command (
        OUTPUT ${_GLSL_OUTPUT}
        COMMAND ${LLC_COMMAND} -I ${LL_GLSL_INCLUDE_DIRS} -o ${_GLSL_OUTPUT} ${SRC}
        MAIN_DEPENDENCY ${SRC}
        COMMENT "Building compute shader ${_GLSL_OUTPUT}"
    )

    set (${TARGET_NAME} ${_GLSL_OUTPUT} PARENT_SCOPE)

endfunction()


# Builds the SPIR-V files from the corresponding GLSL source files.
# 
# PARAMETERS
# - SOURCE_DIR : Source directoy which contains the GLSL files to compile.
# - OUTPUT_DIR : Output directory where the generated SPIR-V files will be written.
# 
function (build_glsl SOURCE_DIR OUTPUT_DIR)
    message (STATUS "build_glsl: SOURCE_DIR: ${SOURCE_DIR}. OUTPUT_DIR: ${OUTPUT_DIR}")

    
    set (LLC_COMMAND ${CMAKE_BINARY_DIR}/tools/llc/llc)
    message (STATUS "LLC_COMMAND: ${LLC_COMMAND}")

    foreach (_SRC ${LL_GLSL_SRCS})
        
        get_filename_component(_GLSL_NAME ${_SRC} NAME)
        get_filename_component(_GLSL_NAME_WE ${_SRC} NAME_WE)

        message (STATUS "glsl source: ${_SRC}\tname: ${_GLSL_NAME}")
        message (STATUS "command: ${LLC_COMMAND} ${_SRC}")

        execute_process (COMMAND ${LLC_COMMAND} -v -o ${CMAKE_BINARY_DIR}/glsl/${_GLSL_NAME_WE}.spv ${_SRC}) 

    endforeach ()
endfunction ()
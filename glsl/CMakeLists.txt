
# create the list of GLSL files to compile
set (_GLSL_SRCS
    assign.comp
    imgToBuffer.comp
)

# LL_GLSL_SRCS contains the list of GLSL source files to be compiled with their absolute path
set (LL_GLSL_SRCS )
foreach (_src ${_GLSL_SRCS})
    list (APPEND LL_GLSL_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/${_src})
endforeach ()

set (LL_GLSL_SRCS ${LL_GLSL_SRCS} PARENT_SCOPE)

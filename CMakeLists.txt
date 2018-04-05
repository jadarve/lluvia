cmake_minimum_required(VERSION 3.5)
project(lluvia)

#################################################
# CMAKE SETTINGS
#################################################

# variable to control de verbosity of the custom functions
set (LL_CMAKE_VERBOSE TRUE)
set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include (Modules/download_file)
include (Modules/llAddShader)


#################################################
# COMPILER SETTINGS
#################################################

message(STATUS "Configuring for Unix system")
set (CMAKE_CXX_COMPILER         "clang")
set (CMAKE_CXX_FLAGS            "-std=c++14 -O0 -Wall")


#################################################
# CATCH SETTINGS
#################################################
enable_testing()
include(cmake/thirdparty/Catch.cmake)


#################################################
# THIRD PARTY SETTINGS
#################################################
include(cmake/thirdparty/JSON.cmake)
include(cmake/thirdparty/STB.cmake)


#################################################
# GLSL COMPILER SETTINGS
#################################################
set (LLC_COMMAND ${CMAKE_BINARY_DIR}/tools/llc/llc)
message (STATUS "LLC_COMMAND: ${LLC_COMMAND}")

#################################################
# VULKAN SETTINGS
#################################################
include_directories($ENV{VULKAN_SDK}/include)
link_directories($ENV{VULKAN_SDK}/lib)


#################################################
# LIBRARY SETTINGS
#################################################
set(LIBRARY_OUTPUT_PATH   "lib")


# TODO: Set this type of variables together in a code block
set (LL_SHADER_OUTPUT_DIR ${CMAKE_BINARY_DIR}/glsl)


#################################################
# SOURCE FILES SETTINGS
#################################################
add_subdirectory(core)

# creates the list of GLSL shaders to build. The list is stored in LL_GLSL_SRCS variable.
add_subdirectory(glsl)
add_subdirectory(tools)
add_subdirectory(samples)


# creates the shaders custom target
include (cmake/shaders.cmake)


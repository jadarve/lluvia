cmake_minimum_required(VERSION 3.5)
project(lluvia)

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
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Catch.cmake)


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
# INCLUDE SETTINGS
#################################################
# include_directories("${PROJECT_SOURCE_DIR}/include")
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/test_macro.cmake)


#################################################
# SOURCE FILES SETTINGS
#################################################
add_subdirectory(core)

# creates the list of GLSL shaders to build. The list is stored in LL_GLSL_SRCS variable.
add_subdirectory(glsl)
add_subdirectory(tools)


# creates the shaders custom target
include (${CMAKE_CURRENT_SOURCE_DIR}/cmake/shaders.cmake)


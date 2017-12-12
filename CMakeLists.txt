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

file(DOWNLOAD
     "https://github.com/catchorg/Catch2/releases/download/v2.0.1/catch.hpp"
     "${CMAKE_BINARY_DIR}/catch/catch.hpp")

set(CATCH_INCLUDE_DIR ${CMAKE_BINARY_DIR})
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${CATCH_INCLUDE_DIR})


#################################################
# VULKAN SETTINGS
#################################################
include_directories($ENV{VULKAN_SDK}/include)
link_directories($ENV{VULKAN_SDK}/lib)


#################################################
# LIBRARY SETTINGS
#################################################
set(LIBRARY_OUTPUT_PATH   "lib")


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

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/build_glsl.cmake)
build_glsl ("Moni moni" "This is the output")

cmake_minimum_required(VERSION 3.7)
project(lluvia)


#################################################
# CMAKE SETTINGS
#################################################
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include (Modules/download_file)
include (Modules/llAddShader)


#################################################
# GLOBAL VARIABLES
#################################################

# Controls de verbosity of the custom functions
set(LL_CMAKE_VERBOSE TRUE)

# GLSL source files and include paths
set(LL_GLSL_SRCS)
set(LL_GLSL_INCLUDE_DIRS)
set(LL_GLSL_OUTPUT_DIR ${CMAKE_BINARY_DIR}/glsl)


#################################################
# COMPILER SETTINGS
#################################################
message(STATUS "-------------------------------------------------")
message(STATUS "Configuring compiler")

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Werror -Wall -Wextra -Wshadow -Wshorten-64-to-32 -Wnon-virtual-dtor -pedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Werror -Wall -Wextra -Wshadow -Wshorten-64-to-32 -Wnon-virtual-dtor -pedantic")
message(STATUS "Compiler : ${CMAKE_CXX_COMPILER}")
message(STATUS "Flags    : ${CMAKE_CXX_FLAGS}")
message(STATUS "-------------------------------------------------")


#################################################
# LIBRARY SETTINGS
#################################################
set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")


#################################################
# THIRD PARTY SETTINGS
#################################################
enable_testing()
include(cmake/thirdparty/Catch.cmake)
include(cmake/thirdparty/JSON.cmake)
include(cmake/thirdparty/STB.cmake)
include(cmake/thirdparty/Lua.cmake)
include(cmake/thirdparty/Sol2.cmake)


#################################################
# GLSL COMPILER SETTINGS
#################################################
set(LLC_COMMAND ${CMAKE_BINARY_DIR}/tools/llc/llc)
message(STATUS "LLC_COMMAND: ${LLC_COMMAND}")


#################################################
# TARGET SYSTEM SETTINGS
#################################################
message(STATUS "-------------------------------------------------")
message(STATUS "Configuring for target system: ${CMAKE_SYSTEM_NAME}")

include(cmake/platforms/${CMAKE_SYSTEM_NAME}.cmake)

message(STATUS "-------------------------------------------------")


#################################################
# GLSL SETTINGS
#################################################
# creates the list of GLSL shaders to build. The list is stored in LL_GLSL_SRCS.
add_subdirectory(glsl)


#################################################
# SOURCE FILES SETTINGS
#################################################
add_subdirectory(core)
add_subdirectory(tools)
add_subdirectory(samples)


# creates the shaders custom target
include (cmake/shaders.cmake)


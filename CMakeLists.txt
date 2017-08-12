cmake_minimum_required(VERSION 2.8.11)
project(lluvia)

#################################################
# TEST SETTINGS
#################################################
# include(ExternalProject)
# ExternalProject_Add(googletest
#   GIT_REPOSITORY    https://github.com/google/googletest.git
#   GIT_TAG           master
#   SOURCE_DIR        "${CMAKE_BINARY_DIR}/googletest-src"
#   BINARY_DIR        "${CMAKE_BINARY_DIR}/googletest-build"
#   CONFIGURE_COMMAND ""
#   BUILD_COMMAND     ""
#   INSTALL_COMMAND   ""
#   TEST_COMMAND      ""
# )


# enable_testing()
# find_package(GTest REQUIRED)
# include_directories(${GTEST_INCLUDE_DIRS})

# # Add googletest directly to our build. This adds
# # the following targets: gtest, gtest_main, gmock
# # and gmock_main
# add_subdirectory("${CMAKE_BINARY_DIR}/googletest-src"
#                  "${CMAKE_BINARY_DIR}/googletest-build")

enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})



# # Prevent GoogleTest from overriding our compiler/linker options
# # when building with Visual Studio
# set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

#################################################
# VULKAN SETTINGS
#################################################
include_directories($ENV{VULKAN_SDK}/include)
link_directories($ENV{VULKAN_SDK}/lib)


#################################################
# LIBRARY SETTINGS
#################################################
set (LIBRARY_OUTPUT_PATH   "lib")


#################################################
# INCLUDE SETTINGS
#################################################
# include_directories("${PROJECT_SOURCE_DIR}/include")
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/test_macro.cmake)


#################################################
# COMPILER SETTINGS
#################################################

message(STATUS "Configuring for Unix system")
set (CMAKE_CXX_COMPILER         "g++")
set (CMAKE_CXX_FLAGS            "-std=c++11 -O3 -Wall")


#################################################
# SOURCE FILES SETTINGS
#################################################
# process the CMakeLists.txt file at src/ folder
# it should add all the source files in the folder
add_subdirectory(core)



# Download Catch directly from Github and configure it as a library.
file (DOWNLOAD
     "https://github.com/catchorg/Catch2/releases/download/v2.0.1/catch.hpp"
     "${CMAKE_BINARY_DIR}/catch/catch.hpp"
)

set (CATCH_INCLUDE_DIR ${CMAKE_BINARY_DIR})
add_library (Catch INTERFACE)
target_include_directories (Catch INTERFACE ${CATCH_INCLUDE_DIR})

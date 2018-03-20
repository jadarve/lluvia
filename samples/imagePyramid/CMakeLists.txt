message (STATUS "-------------------------------------------------")
message (STATUS "Building imagePyramid")

add_executable(imagePyramid
    src/ImagePyramid.cpp
    src/main.cpp
)

# TODO: need to add shader compilation target

# message (STATUS "ImagePyramid: LL_INCLUDE_DIRS: ${LL_INCLUDE_DIRS}")
# message (STATUS "ImagePyramid: LL_LINK_LIBRARIES: ${LL_LINK_LIBRARIES}")

target_include_directories(imagePyramid
    PUBLIC ${LL_INCLUDE_DIRS}
    PUBLIC include
)

target_link_libraries(imagePyramid ${LL_LINK_LIBRARIES})

message (STATUS "-------------------------------------------------")

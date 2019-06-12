
set (LL_CORE_TEST_DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test/data)

set (_TEST_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test/include)


function(add_catchTest NAME SRCS)
    
    add_executable (${NAME} ${SRCS})
    target_include_directories(${NAME} PRIVATE ${_TEST_INCLUDE_DIR})
    target_link_libraries (${NAME} m Catch vulkan stdc++ ${ARGV2})
    add_test (NAME ${NAME}  COMMAND ${NAME})

endfunction()

set(LL_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(LL_LINK_LIBRARIES stdc++ json vulkan)

set(_SRCS
    src/Buffer.cpp
    src/CommandBuffer.cpp
    src/ComputeNode.cpp
    src/ComputeNodeDescriptor.cpp
    src/error.cpp
    src/Image.cpp
    src/ImageDescriptor.cpp
    src/ImageView.cpp
    src/ImageViewDescriptor.cpp
    src/impl/MemoryFreeSpaceManager.cpp
    src/Memory.cpp
    src/MemoryAllocationInfo.cpp
    src/new/ComputeNode.cpp
    src/new/Node.cpp
    src/Program.cpp
    src/Session.cpp
    src/utils.cpp
)


add_library (lluvia-core        STATIC ${_SRCS})
set_property(TARGET lluvia-core PROPERTY POSITION_INDEPENDENT_CODE ON)

target_include_directories(lluvia-core
    PUBLIC ${LL_INCLUDE_DIRS}
    PUBLIC ${JSON_INCLUDE_DIR}
)

target_link_libraries(lluvia-core ${LL_LINK_LIBRARIES})


# configure
configure_file (${_TEST_INCLUDE_DIR}/coreTestConfig.h.in ${_TEST_INCLUDE_DIR}/coreTestConfig.h)

add_catchTest (test_VulkanDriver             test/test_VulkanDriver.cpp)
add_catchTest (test_Base64                   test/test_Base64.cpp                    lluvia-core)
add_catchTest (test_MemoryFreeSpaceManager   test/test_MemoryFreeSpaceManager.cpp    lluvia-core)
add_catchTest (test_SessionCreation          test/test_SessionCreation.cpp           lluvia-core)
add_catchTest (test_BufferCreation           test/test_BufferCreation.cpp            lluvia-core)
add_catchTest (test_BufferCopy               test/test_BufferCopy.cpp                lluvia-core)
add_catchTest (test_BufferMapping            test/test_BufferMapping.cpp             lluvia-core)
add_catchTest (test_ImageCreation            test/test_ImageCreation.cpp             lluvia-core)
add_catchTest (test_ProgramCreation          test/test_ProgramCreation.cpp           lluvia-core)
add_catchTest (test_ComputeNode              test/test_ComputeNode.cpp               lluvia-core)
add_catchTest (test_ComputeNodeImage         test/test_ComputeNodeImage.cpp          lluvia-core)
add_catchTest (test_utils                    test/test_utils.cpp                     lluvia-core)


# store variables in the cache
set(LL_INCLUDE_DIRS ${LL_INCLUDE_DIRS} CACHE INTERNAL "Lluvia include directories")
set(LL_LINK_LIBRARIES lluvia-core ${LL_LINK_LIBRARIES} CACHE INTERNAL "Lluvia link libraries")

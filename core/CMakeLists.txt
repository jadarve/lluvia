
set (_TEST_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test/include)

function(add_catchTest NAME SRCS)
    
    add_executable (${NAME} ${SRCS})
    target_include_directories(${NAME} PRIVATE ${_TEST_INCLUDE_DIR})
    target_link_libraries (${NAME} Catch vulkan stdc++ ${ARGV2})
    add_test (NAME ${NAME}  COMMAND ${NAME})

endfunction()


include_directories (
    include
)

add_library (lluvia-core SHARED
    src/Buffer.cpp
    src/CommandBuffer.cpp
    src/ComputeNode.cpp
    src/ComputeNodeDescriptor.cpp
    src/ComputeGraph.cpp
    src/ComputeGraphFileWriter.cpp
    src/impl/MemoryFreeSpaceManager.cpp
    src/impl/OneTimeSubmitCommandBuffer.cpp
    src/Memory.cpp
    src/Program.cpp
    src/Session.cpp
    src/utils.cpp
)

target_link_libraries (lluvia-core
    json
    vulkan
)


# configure
configure_file (${_TEST_INCLUDE_DIR}/coreTestConfig.h.in ${_TEST_INCLUDE_DIR}/coreTestConfig.h)

add_catchTest (test_VulkanDriver             test/test_VulkanDriver.cpp)
add_catchTest (test_MemoryFreeSpaceManager   test/test_MemoryFreeSpaceManager.cpp    lluvia-core)
add_catchTest (test_SessionCreation          test/test_SessionCreation.cpp           lluvia-core)
add_catchTest (test_BufferCreation           test/test_BufferCreation.cpp            lluvia-core)
add_catchTest (test_BufferCopy               test/test_BufferCopy.cpp                lluvia-core)
add_catchTest (test_ProgramCreation          test/test_ProgramCreation.cpp           lluvia-core)
add_catchTest (test_ComputeNode              test/test_ComputeNode.cpp               lluvia-core)
add_catchTest (test_ComputeGraphWriter       test/test_ComputeGraphWriter.cpp        lluvia-core)

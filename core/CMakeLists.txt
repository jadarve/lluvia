
test_macro()


function(add_catchTest NAME SRCS)
    
    add_executable(${NAME} ${SRCS})
    target_link_libraries(${NAME} Catch vulkan stdc++ ${ARGV2})
    add_test(NAME ${NAME}  COMMAND ${NAME})
endfunction()


include_directories(include)
add_library(lluvia-core SHARED
    src/Buffer.cpp
    src/ComputeNode.cpp
    src/ComputeNodeDescriptor.cpp
    src/impl/MemoryFreeSpaceManager.cpp
    src/Memory.cpp
    src/Program.cpp
    src/Session.cpp
)

target_link_libraries(lluvia-core vulkan)

add_catchTest(test_VulkanDriver             test/test_VulkanDriver.cpp)
add_catchTest(test_MemoryFreeSpaceManager   test/test_MemoryFreeSpaceManager.cpp    lluvia-core)
add_catchTest(test_SessionCreation          test/test_SessionCreation.cpp           lluvia-core)
add_catchTest(test_BufferCreation           test/test_BufferCreation.cpp            lluvia-core)
add_catchTest(test_ProgramCreation          test/test_ProgramCreation.cpp           lluvia-core)
add_catchTest(test_ComputeNode              test/test_ComputeNode.cpp               lluvia-core)

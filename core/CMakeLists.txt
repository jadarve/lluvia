
test_macro()


function(add_catchTest NAME SRCS)
    
    add_executable(${NAME} ${SRCS})
    target_link_libraries(${NAME} Catch vulkan stdc++ ${ARGV2})
    add_test(NAME ${NAME}  COMMAND ${NAME})
endfunction()


include_directories(include)
add_library(lluvia-core SHARED
    src/Session.cpp
    src/Buffer.cpp
    src/Memory.cpp
    src/Shader.cpp
    src/impl/MemoryFreeSpaceManager.cpp
)

target_link_libraries(lluvia-core vulkan)

add_catchTest(test_VulkanDriver             test/test_VulkanDriver.cpp)
add_catchTest(test_MemoryFreeSpaceManager   test/test_MemoryFreeSpaceManager.cpp    lluvia-core)
add_catchTest(test_SessionCreation          test/test_SessionCreation.cpp           lluvia-core)
add_catchTest(test_BufferCreation           test/test_BufferCreation.cpp            lluvia-core)

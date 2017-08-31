
test_macro()

function(add_gtest NAME SRCS)

    set(args_list --gtest_output=xml:test/${NAME}.xml)
    add_executable(${NAME} ${SRCS})
    target_link_libraries(${NAME} vulkan lluvia-core ${GTEST_BOTH_LIBRARIES} pthread)
    GTEST_ADD_TESTS(${NAME} "${args_list}" ${SRCS})

endfunction()


include_directories(include)
add_library(lluvia-core SHARED
    src/Session.cpp
    src/Buffer.cpp
    src/Memory.cpp
    src/impl/MemoryHeap.cpp
    src/impl/MemoryFreeSpaceManager.cpp
)

target_link_libraries(lluvia-core vulkan)


# Add test cpp file
# add_executable(runUnitTests test/test_Test.cpp)
# target_link_libraries(runUnitTests gtest_main)
# add_test(NAME runUnitTests COMMAND runUnitTests)


# add_executable(vulkanDriverTest test/test_VulkanDriver.cpp)
# target_link_libraries(vulkanDriverTest vulkan ${GTEST_BOTH_LIBRARIES} pthread)
# add_test(NAME vulkanDriverTest COMMAND vulkanDriverTest)

add_gtest(test_MemoryFreeSpaceManager test/test_MemoryFreeSpaceManager.cpp)


# add_executable(sessionTest test/test_SessionCreation.cpp)
# target_link_libraries(sessionTest vulkan lluvia-core ${GTEST_BOTH_LIBRARIES} pthread)
# add_test(NAME sessionTest COMMAND sessionTest)

# add_executable(minimumComputeGraph test/test_MinimumComputeGraph.cpp)
# target_link_libraries(minimumComputeGraph vulkan ${GTEST_BOTH_LIBRARIES} pthread)
# add_test(NAME minimumComputeGraph COMMAND minimumComputeGraph)
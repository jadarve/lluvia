
test_macro()

include_directories(include)
add_library(lluviacore SHARED
    src/Session.cpp
    src/Buffer.cpp
    src/Memory.cpp
    src/impl/MemoryHeap.cpp
)

target_link_libraries(lluviacore vulkan)

# add_executable(lluviacore src/main.cpp)

# Add test cpp file
# add_executable(runUnitTests test/test_Test.cpp)
# target_link_libraries(runUnitTests gtest_main)
# add_test(NAME runUnitTests COMMAND runUnitTests)

add_executable(sessionTest test/test_SessionCreation.cpp)
target_link_libraries(sessionTest vulkan lluviacore ${GTEST_BOTH_LIBRARIES} pthread)
add_test(NAME sessionTest COMMAND sessionTest)


add_executable(vulkanDriverTest test/test_VulkanDriver.cpp)
target_link_libraries(vulkanDriverTest vulkan ${GTEST_BOTH_LIBRARIES} pthread)
add_test(NAME vulkanDriverTest COMMAND vulkanDriverTest)


# add_executable(minimumComputeGraph test/test_MinimumComputeGraph.cpp)
# target_link_libraries(minimumComputeGraph vulkan ${GTEST_BOTH_LIBRARIES} pthread)
# add_test(NAME minimumComputeGraph COMMAND minimumComputeGraph)
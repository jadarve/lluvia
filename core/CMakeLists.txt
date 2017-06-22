
test_macro()

add_executable(lluvia-core src/main.cpp)

# Add test cpp file
# add_executable(runUnitTests test/test_Test.cpp)
# target_link_libraries(runUnitTests gtest_main)
# add_test(NAME runUnitTests COMMAND runUnitTests)


add_executable(vulkanDriverTest test/test_VulkanDriver.cpp)
target_link_libraries(vulkanDriverTest gtest_main vulkan)
add_test(NAME vulkanDriverTest COMMAND vulkanDriverTest)
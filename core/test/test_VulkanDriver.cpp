/**
 * \file test_VulkanDriver.cpp
 * \brief test Vulkan driver usage
 * \copyright 2017, Juan David Adarve. See AUTHORS for more details
 * \license Apache 2.0, see LICENSE for more details
 */

#include <iostream>
#include <gtest/gtest.h>
#include <vulkan/vulkan.hpp>

/**
 * Test Vulkan instance creation.
 */
TEST(VulkanDriverTest, CreateInstance) {

    const vk::ApplicationInfo appInfo = vk::ApplicationInfo()
            .setPApplicationName("lluvia")
            .setApplicationVersion(0)
            .setEngineVersion(0)
            .setPEngineName("lluvia");

    const vk::InstanceCreateInfo instanceInfo = vk::InstanceCreateInfo()
            .setPApplicationInfo(&appInfo);


    vk::Instance instance;
    vk::Result result = vk::createInstance(&instanceInfo, nullptr, &instance);

    // check that the instance is created successfully
    ASSERT_EQ(result, vk::Result::eSuccess);
}


int main(int argc, char **argv) {
    
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
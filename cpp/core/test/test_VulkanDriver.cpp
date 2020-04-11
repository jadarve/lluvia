/**
 * \file test_VulkanDriver.cpp
 * \brief test Vulkan driver usage
 * \copyright 2017, Juan David Adarve. See AUTHORS for more details
 * \license Apache 2.0, see LICENSE for more details
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include <iostream>
#include <vulkan/vulkan.hpp>


TEST_CASE("create_instance", "test_VulkanDriver") {

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
    REQUIRE(result == vk::Result::eSuccess);

    instance.destroy();
}


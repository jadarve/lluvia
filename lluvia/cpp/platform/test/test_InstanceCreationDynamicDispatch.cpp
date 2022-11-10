/**
 * \file test_VulkanInstance.cpp
 * \brief test Vulkan instance creation
 * \copyright 2021, Juan David Adarve. See AUTHORS for more details
 * \license Apache 2.0, see LICENSE for more details
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

// define this macro to use Vulkan's dynamic dispatcher by default
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.hpp>

// reserve space for the dynamic dispatch loader function pointers.
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

TEST_CASE("create_instance", "test_InstanceCreationDynamicDispatch")
{

    auto dl                    = vk::DynamicLoader {};
    auto vkGetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

    auto appInfo = vk::ApplicationInfo()
                       .setPApplicationName("lluvia")
                       .setApplicationVersion(0)
                       .setEngineVersion(0)
                       .setPEngineName("lluvia")
                       .setApiVersion(VK_MAKE_VERSION(1u, 2u, 176u));

    const vk::InstanceCreateInfo instanceInfo = vk::InstanceCreateInfo()
                                                    .setPApplicationInfo(&appInfo);

    vk::Instance instance;
    vk::Result   result = vk::createInstance(&instanceInfo, nullptr, &instance);
    REQUIRE(result == vk::Result::eSuccess);

    VULKAN_HPP_DEFAULT_DISPATCHER.init(instance);
    REQUIRE(VULKAN_HPP_DEFAULT_DISPATCHER.vkDestroyInstance != nullptr);

    instance.destroy();
}

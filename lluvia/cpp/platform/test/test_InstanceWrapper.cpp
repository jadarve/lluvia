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

#include <memory>

// This class mimics the way ll::vulkan::Instance works
class InstanceWrapper {

public:
    InstanceWrapper()
    {

        auto vkGetInstanceProcAddr = m_loader.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
        VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

        auto appInfo = vk::ApplicationInfo()
                           .setPApplicationName("lluvia")
                           .setApplicationVersion(0)
                           .setEngineVersion(0)
                           .setPEngineName("lluvia")
                           .setApiVersion(VK_MAKE_VERSION(1u, 2u, 176u));

        const vk::InstanceCreateInfo instanceInfo = vk::InstanceCreateInfo()
                                                        .setPApplicationInfo(&appInfo);

        vk::Result result = vk::createInstance(&instanceInfo, nullptr, &m_instance);
        REQUIRE(result == vk::Result::eSuccess);

        VULKAN_HPP_DEFAULT_DISPATCHER.init(m_instance);
        REQUIRE(VULKAN_HPP_DEFAULT_DISPATCHER.vkDestroyInstance != nullptr);
    }

    ~InstanceWrapper()
    {

        m_instance.destroy();
    }

private:
    vk::DynamicLoader m_loader {};
    vk::Instance m_instance {};
};

TEST_CASE("create_instance_wrapper", "test_InstanceCreationDynamicDispatch")
{

    {
        auto instance = std::make_unique<InstanceWrapper>();
    }
}

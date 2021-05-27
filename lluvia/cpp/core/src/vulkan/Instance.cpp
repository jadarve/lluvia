/**
@file       Instance.cpp
@brief      Instance class.
@copyright  2021, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

// define this macro to use Vulkan's dynamic dispatcher by default
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include "lluvia/core/vulkan/Instance.h"

#include "lluvia/core/error.h"
#include "lluvia/core/debug_utils.h"

// reserve space for the dynamic dispatch loader function pointers.
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

#include <iostream>

namespace ll::vulkan {

Instance::Instance(bool debugEnabled):
    m_loader {},
    m_debugEnabled {debugEnabled} {
    
    auto vkGetInstanceProcAddr = m_loader.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
    
    auto appInfo = vk::ApplicationInfo()
            .setPApplicationName("lluvia")
            .setApplicationVersion(0)
            .setEngineVersion(0)
            .setPEngineName("lluvia")
            .setApiVersion(VK_MAKE_VERSION(1u, 2u, 176u));

    const auto extensions = getRequiredExtensionNames();
    const auto layers = getRequiredLayersNames();

    vk::InstanceCreateInfo instanceInfo = vk::InstanceCreateInfo()
            .setEnabledExtensionCount(static_cast<uint32_t>(extensions.size()))
            .setPpEnabledExtensionNames(&extensions[0])
            .setEnabledLayerCount(static_cast<uint32_t>(layers.size()))
            .setPpEnabledLayerNames(&layers[0])
            .setPApplicationInfo(&appInfo);

    const auto result = vk::createInstance(&instanceInfo, nullptr, &m_instance);
    ll::throwSystemErrorIf(result != vk::Result::eSuccess, ll::ErrorCode::InstanceCreationError, "error creating instance: " + vk::to_string(result));

    VULKAN_HPP_DEFAULT_DISPATCHER.init(m_instance);

    if (m_debugEnabled) {

        ll::throwSystemErrorIf(VULKAN_HPP_DEFAULT_DISPATCHER.vkCreateDebugUtilsMessengerEXT == nullptr,
                               ErrorCode::InstanceCreationError, "error loading vkCreateDebugUtilsMessengerEXT using a DispatchLoaderDynamic");
        ll::throwSystemErrorIf(VULKAN_HPP_DEFAULT_DISPATCHER.vkDestroyDebugUtilsMessengerEXT == nullptr,
                               ErrorCode::InstanceCreationError, "error loading vkDestroyDebugUtilsMessengerEXT using a DispatchLoaderDynamic");

        m_debugMessenger = m_instance.createDebugUtilsMessengerEXT(
                vk::DebugUtilsMessengerCreateInfoEXT{{},
                                                     vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
                                                     vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                                                     vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                                                     vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo,
                                                     vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                                                     vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                                                     vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
                                                     &ll::debugCallback,
                                                     nullptr});
    }
}


Instance::~Instance() {

    if (m_debugEnabled) {
        m_instance.destroyDebugUtilsMessengerEXT(m_debugMessenger);
    }

    m_instance.destroy();
}


vk::Instance& Instance::get() noexcept {
    return m_instance;
}

std::vector<const char*> Instance::getRequiredLayersNames() {

    constexpr const auto layerNames = std::array<const char*, 1>{"VK_LAYER_KHRONOS_validation"};

    auto layers = std::vector<const char*>{};

    if (m_debugEnabled) {

        const auto availableLayers = vk::enumerateInstanceLayerProperties(VULKAN_HPP_DEFAULT_DISPATCHER);

        for (const auto& layerName : layerNames) {

            auto predicate = [&layerName](const vk::LayerProperties& props) { return std::string(static_cast<const char*>(props.layerName)) == layerName;};
            const auto it = std::find_if(std::begin(availableLayers), std::end(availableLayers), predicate);

            if (it != std::end(availableLayers)) {
                layers.push_back(layerName);
            }
        }

        ll::throwSystemErrorIf(layers.size() != layerNames.size(), ll::ErrorCode::LayerNotFound, "some validation layer not found");

    }

    return layers;
}

std::vector<const char*> Instance::getRequiredExtensionNames() {

    constexpr const auto debugUtilsExtensionName = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;

    auto extensions = std::vector<const char*>{};

    if (m_debugEnabled) {

        const auto availableExtensions = vk::enumerateInstanceExtensionProperties(std::string{"VK_LAYER_KHRONOS_validation"}, VULKAN_HPP_DEFAULT_DISPATCHER);

        auto predicate = [&](const vk::ExtensionProperties& props) { return std::string(static_cast<const char*>(props.extensionName)) == debugUtilsExtensionName;};

        auto it = std::find_if(std::begin(availableExtensions), std::end(availableExtensions), predicate);
        ll::throwSystemErrorIf(it == std::end(availableExtensions), ll::ErrorCode::ExtensionNotFound, std::string("extension not found: ") + debugUtilsExtensionName);

        extensions.push_back(debugUtilsExtensionName);
    }

    return extensions;
}

} // namespace ll::vulkan

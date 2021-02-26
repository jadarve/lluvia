#include "lluvia/core/vulkan/Instance.h"

#include "lluvia/core/error.h"
#include "lluvia/core/debug_utils.h"

#include <iostream>

namespace ll::vulkan {

Instance::Instance(bool debugEnabled):
    m_debugEnabled{debugEnabled} {

    auto appInfo = vk::ApplicationInfo()
            .setPApplicationName("lluvia")
            .setApplicationVersion(0)
            .setEngineVersion(0)
            .setPEngineName("lluvia")
            .setApiVersion(VK_MAKE_VERSION(1u, 2u, 162u));

    const auto extensions = getRequiredExtensionNames();
    const auto layers = getRequiredLayersNames();

    vk::InstanceCreateInfo instanceInfo = vk::InstanceCreateInfo()
            .setEnabledExtensionCount(extensions.size())
            .setPpEnabledExtensionNames(&extensions[0])
            .setEnabledLayerCount(layers.size())
            .setPpEnabledLayerNames(&layers[0])
            .setPApplicationInfo(&appInfo);

    const auto result = vk::createInstance(&instanceInfo, nullptr, &m_instance);
    ll::throwSystemErrorIf(result != vk::Result::eSuccess, ll::ErrorCode::InstanceCreationError, "error creating instance: " + vk::to_string(result));

    if (m_debugEnabled) {

#ifdef __ANDROID__
        m_dispatchLoaderDynamic = vk::DispatchLoaderDynamic();
        m_dispatchLoaderDynamic.init(m_instance);
#else
        m_dispatchLoaderDynamic = vk::DispatchLoaderDynamic(m_instance, vkGetInstanceProcAddr);
        m_dispatchLoaderDynamic.init(m_instance, vkGetInstanceProcAddr);
#endif
        ll::throwSystemErrorIf(m_dispatchLoaderDynamic.vkCreateDebugUtilsMessengerEXT == nullptr,
                               ErrorCode::InstanceCreationError, "error loading vkCreateDebugUtilsMessengerEXT using a DispatchLoaderDynamic");
        ll::throwSystemErrorIf(m_dispatchLoaderDynamic.vkDestroyDebugUtilsMessengerEXT == nullptr,
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
                                                     ll::debugCallback},
                nullptr, m_dispatchLoaderDynamic);
    }
}


Instance::~Instance() {

    if (m_debugEnabled) {
        m_instance.destroyDebugUtilsMessengerEXT(m_debugMessenger, nullptr, m_dispatchLoaderDynamic);
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

        const auto availableLayers = vk::enumerateInstanceLayerProperties();

        for (const auto& layerName : layerNames) {

            auto predicate = [&layerName](const vk::LayerProperties& props) { return std::string(props.layerName) == layerName;};
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

    constexpr const auto debugUtilsExtensionName = "VK_EXT_debug_utils";

    auto extensions = std::vector<const char*>{};

    if (m_debugEnabled) {

        const auto availableExtensions = vk::enumerateInstanceExtensionProperties();

        auto predicate = [](const vk::ExtensionProperties& props) { return std::string(props.extensionName) == debugUtilsExtensionName;};

        auto it = std::find_if(std::begin(availableExtensions), std::end(availableExtensions), predicate);
        ll::throwSystemErrorIf(it == std::end(availableExtensions), ll::ErrorCode::ExtensionNotFound, std::string("extension not found: ") + debugUtilsExtensionName);

        extensions.push_back(debugUtilsExtensionName);
    }

    return extensions;
}

} // namespace ll::vulkan

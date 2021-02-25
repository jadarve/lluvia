#include "lluvia/core/debug_utils.h"

#include <atomic>
#include <iostream>

namespace ll {

auto messagesReceived = std::atomic_bool{false};

bool hasReceivedVulkanWarningMessages() {

    return messagesReceived;
}

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {

    if (messageSeverity >= VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        messagesReceived.store(true, std::memory_order_release);

        // TODO: For android, use log library
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
    }

    return VK_FALSE;
}

} // namespace ll

#include "lluvia/core/debug_utils.h"

#include <atomic>
#include <iostream>

#ifdef __ANDROID__
#include <android/log.h>
#endif

namespace ll {

auto messagesReceived = std::atomic_bool{false};

bool hasReceivedVulkanWarningMessages() {

    return messagesReceived;
}

#ifdef __ANDROID__

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData) {

    // from https://developer.android.com/ndk/guides/graphics/validation-layer

    if (messageSeverity >= VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        messagesReceived.store(true, std::memory_order_release);
    }

    const char validation[]      = "Validation";
    const char performance[]     = "Performance";
    const char error[]           = "ERROR";
    const char warning[]         = "WARNING";
    const char unknownType[]     = "UNKNOWN_TYPE";
    const char unknownSeverity[] = "UNKNOWN_SEVERITY";
    const char* typeString       = unknownType;
    const char* severityString   = unknownSeverity;
    const char* messageIdName    = pCallbackData->pMessageIdName;
    int32_t messageIdNumber      = pCallbackData->messageIdNumber;
    const char* message          = pCallbackData->pMessage;
    android_LogPriority priority = ANDROID_LOG_UNKNOWN;

    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        severityString = error;
        priority = ANDROID_LOG_ERROR;
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        severityString = warning;
        priority = ANDROID_LOG_WARN;
    }

    if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {
        typeString = validation;
    } else if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) {
        typeString = performance;
    }

    __android_log_print(priority,
                       "Lluvia",
                       "%s %s: [%s] Code %i : %s",
                       typeString,
                       severityString,
                       messageIdName,
                       messageIdNumber,
                       message);

    return VK_FALSE;
}

#else // Other Operating Systems

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        [[maybe_unused]] void* pUserData) {

    if (messageSeverity >= VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        messagesReceived.store(true, std::memory_order_release);
    }

    if (messageSeverity >= VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        std::cerr << "Lluvia: " << pCallbackData->pMessage << std::endl;
    }

    return VK_FALSE;
}

#endif

} // namespace ll

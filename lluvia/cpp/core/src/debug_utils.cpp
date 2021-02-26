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

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {

    if (messageSeverity >= VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        messagesReceived.store(true, std::memory_order_release);

        // TODO: For android, use log library
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
    }

#ifdef __ANDROID__
    // from https://developer.android.com/ndk/guides/graphics/validation-layer

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
    }
    else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        severityString = warning;
        priority = ANDROID_LOG_WARN;
    }
    if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {
        typeString = validation;
    }
    else if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) {
        typeString = performance;
    }

    __android_log_print(priority,
                       "AppName",
                       "%s %s: [%s] Code %i : %s",
                       typeString,
                       severityString,
                       messageIdName,
                       messageIdNumber,
                       message);
#endif

    return VK_FALSE;
}

} // namespace ll

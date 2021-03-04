/**
@file       debug_utils.h
@brief      Debug util methods.
@copyright  2021, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_DEBUG_UTILS_H_
#define LLUVIA_CORE_DEBUG_UTILS_H_

#include <vulkan/vulkan.hpp>

namespace ll {

/**
 @brief returns whether or not the application has received messages through the debug callback.
 */
bool hasReceivedVulkanWarningMessages();

/**
 @brief debug callback function to receive messages from the vulkan debug messenger.

 @param messageSeverity
 @param messageType
 @param pCallbackData
 @param pUserData
 @return false to continue the execution of the app.
 */
VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

} // namespace ll

#endif // LLUVIA_CORE_DEBUG_UTILS_H_


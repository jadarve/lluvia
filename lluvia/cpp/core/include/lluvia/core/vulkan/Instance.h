/**
@file       Instance.h
@brief      Instance class.
@copyright  2020, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_VULKAN_INSTANCE_H_
#define LLUVIA_CORE_VULKAN_INSTANCE_H_

#include <atomic>
#include <memory>

#include "lluvia/core/vulkan/vulkan.hpp"

namespace ll {
namespace vulkan {

    class Instance {

    public:
        /**
         @brief returns whether or not the application has received messages through the debug callback.
        */
        static bool hasReceivedVulkanWarningMessages();

        /**
        @brief debug callback function to receive messages from the vulkan debug messenger.

        @param messageSeverity
        @param messageType
        @param pCallbackData
        @param pUserData
        @return false to continue the execution of the app.
        */
        static VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

    public:
        Instance() = delete;
        Instance(const Instance& instance) = delete;
        Instance(Instance&& instance) = delete;

        Instance(bool debugEnabled);
        ~Instance();

        Instance& operator=(const Instance& Instance) = delete;
        Instance& operator=(Instance&& Instance) = delete;

        vk::Instance& get() noexcept;

    private:
        static std::atomic_bool m_hasReceivedVulkanWarningMessages;

    private:
        std::vector<const char*> getRequiredLayersNames();
        std::vector<const char*> getRequiredExtensionNames();

        vk::DynamicLoader m_loader;
        vk::Instance m_instance;
        vk::DebugUtilsMessengerEXT m_debugMessenger;
        bool m_debugEnabled;
    };

} // namespace vk
} // namespace ll

#endif // LLUVIA_CORE_VULKAN_INSTANCE_H_

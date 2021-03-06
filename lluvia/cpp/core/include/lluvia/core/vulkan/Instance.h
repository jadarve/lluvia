/**
@file       Instance.h
@brief      Instance class.
@copyright  2020, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_VULKAN_INSTANCE_H_
#define LLUVIA_CORE_VULKAN_INSTANCE_H_

#include <memory>

#include "lluvia/core/vulkan/vulkan.hpp"

namespace ll {
namespace vulkan {

class Instance {

public:
    Instance() = delete;
    Instance(const Instance &instance) = delete;
    Instance(Instance &&instance) = delete;

    Instance(bool debugEnabled);
    ~Instance();

    Instance &operator=(const Instance &Instance) = delete;
    Instance &operator=(Instance &&Instance) = delete;


    vk::Instance& get() noexcept;

private:
    std::vector<const char*> getRequiredLayersNames();
    std::vector<const char*> getRequiredExtensionNames();

    vk::Instance                 m_instance;
    vk::DispatchLoaderDynamic    m_dispatchLoaderDynamic;
    vk::DebugUtilsMessengerEXT   m_debugMessenger;
    bool                         m_debugEnabled;
};

} // namespace vk
} // namespace ll

#endif // LLUVIA_CORE_VULKAN_INSTANCE_H_

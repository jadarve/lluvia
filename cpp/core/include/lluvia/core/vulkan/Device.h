/**
@file       Device.h
@brief      Device class.
@copyright  2020, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_VULKAN_DEVICE_H_
#define LLUVIA_CORE_VULKAN_DEVICE_H_

#include <memory>


#include <vulkan/vulkan.hpp>


namespace ll {
namespace vulkan {

class Instance;

class Device {

public:
    Device() = delete;
    Device(const Device& device) = delete;
    Device(Device&& device) = delete;

    Device(const vk::Device& device, const std::shared_ptr<ll::vulkan::Instance>& instance);
    ~Device();

    Device& operator = (const Device& device) = delete;
    Device& operator = (Device&& device) = delete;

    vk::Device& get() noexcept;

private:
    vk::Device m_device;
    std::shared_ptr<ll::vulkan::Instance> m_instance;
};


} // namespace vulkan
} // namespace ll

#endif // LLUVIA_CORE_VULKAN_DEVICE_H_

/**
@file       Device.h
@brief      Device class.
@copyright  2020, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_VULKAN_DEVICE_H_
#define LLUVIA_CORE_VULKAN_DEVICE_H_

#include <memory>

#include "lluvia/core/ComputeDimension.h"
#include "lluvia/core/types.h"
#include "lluvia/core/vulkan/vulkan.hpp"

namespace ll {

class ImageDescriptor;
class CommandBuffer;

namespace vulkan {

    ll::vec3ui computeOptimalLocalShape(ll::ComputeDimension dimension, uint32_t maxInvocations, const ll::vec3ui& maxSize);

    // forward declaration
    class Instance;

    class Device : public std::enable_shared_from_this<ll::vulkan::Device> {

    public:
        Device()                     = delete;
        Device(const Device& device) = delete;
        Device(Device&& device)      = delete;

        Device(const vk::Device&                         device,
            const vk::PhysicalDevice&                    physicalDevice,
            const uint32_t                               computeQueueFamilyIndex,
            const std::shared_ptr<ll::vulkan::Instance>& instance);
        ~Device();

        Device& operator=(const Device& device) = delete;
        Device& operator=(Device&& device)      = delete;

        vk::Device&                     get() noexcept;
        vk::PhysicalDevice&             getPhysicalDevice() noexcept;
        const vk::PhysicalDeviceLimits& getPhysicalDeviceLimits() noexcept;
        vk::CommandPool&                getCommandPool() noexcept;
        uint32_t                        getComputeFamilyQueueIndex() const noexcept;

        ll::vec3ui getComputeLocalShape(ll::ComputeDimension dimension) const noexcept;

        bool isImageDescriptorSupported(const ll::ImageDescriptor& descriptor) const noexcept;

        std::unique_ptr<ll::CommandBuffer> createCommandBuffer();

        void run(const ll::CommandBuffer& cmdBuffer);

    private:
        vk::Device               m_device;
        vk::PhysicalDevice       m_physicalDevice;
        vk::PhysicalDeviceLimits m_physicalDeviceLimits;
        vk::CommandPool          m_commandPool;

        vk::Queue m_queue;
        uint32_t  m_computeQueueFamilyIndex;

        // cached local compute shapes for each compute dimension
        ll::vec3ui m_localComputeShapeD1;
        ll::vec3ui m_localComputeShapeD2;
        ll::vec3ui m_localComputeShapeD3;

        // reference to the instance this device was created from
        std::shared_ptr<ll::vulkan::Instance> m_instance;
    };

} // namespace vulkan
} // namespace ll

#endif // LLUVIA_CORE_VULKAN_DEVICE_H_

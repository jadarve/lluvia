#include "lluvia/core/vulkan/Device.h"

#include "lluvia/core/CommandBuffer.h"
#include "lluvia/core/error.h"
#include "lluvia/core/image/ImageDescriptor.h"
#include "lluvia/core/image/ImageTiling.h"
#include "lluvia/core/image/ImageUsageFlags.h"

#include <algorithm>

namespace ll::vulkan {

ll::vec3ui computeOptimalLocalShape(ll::ComputeDimension dimension, uint32_t maxInvocations, const ll::vec3ui& maxSize)
{
    switch (dimension) {
    case ll::ComputeDimension::D1:

        return ll::vec3ui {maxSize.x, 1, 1};

    case ll::ComputeDimension::D2: {
        auto current = ll::vec3ui {1, 1, 1};
        auto next    = ll::vec3ui {1, 1, 1};
        auto volume  = next.x * next.y * next.z;

        // increase the X and Y size until the volume exceeds the max number of invocations
        // or any dimension is greater than the maxSize
        while (volume <= maxInvocations && next.x <= maxSize.x && next.y <= maxSize.y) {

            current = next;
            next    = ll::vec3ui {next.x + 1, next.y + 1, 1};
            volume  = next.x * next.y * next.z;
        }

        return current;
    }

    case ll::ComputeDimension::D3: {

        // fix the value of Z to 4, then try to find good values for X and Y
        auto fixedZ = std::min(4u, maxSize.z);

        auto current = ll::vec3ui {1, 1, fixedZ};
        auto next    = ll::vec3ui {1, 1, fixedZ};
        auto volume  = next.x * next.y * next.z;

        // increase the X and Y size until the volume exceeds the max number of invocations
        // or any dimension is greater than the maxSize
        while (volume <= maxInvocations && next.x <= maxSize.x && next.y <= maxSize.y) {

            current = next;
            next    = ll::vec3ui {next.x + 1, next.y + 1, fixedZ};
            volume  = next.x * next.y * next.z;
        }

        return current;
    }
    }
}

Device::Device(const vk::Device&                 device,
    const vk::PhysicalDevice&                    physicalDevice,
    const uint32_t                               computeQueueFamilyIndex,
    const std::shared_ptr<ll::vulkan::Instance>& instance)
    : m_device {device}
    , m_physicalDevice {physicalDevice}
    , m_computeQueueFamilyIndex {computeQueueFamilyIndex}
    , m_instance {instance}
{

    m_physicalDeviceLimits    = m_physicalDevice.getProperties().limits;
    m_computeQueueFamilyIndex = getComputeFamilyQueueIndex();

    const auto createInfo = vk::CommandPoolCreateInfo()
                                .setQueueFamilyIndex(m_computeQueueFamilyIndex);

    m_commandPool = m_device.createCommandPool(createInfo);
    m_queue       = m_device.getQueue(m_computeQueueFamilyIndex, 0);

    /////////////////////////////////////////////////////
    // compute optimal compute shapes for all dimensions
    const auto maxInvocations = m_physicalDeviceLimits.maxComputeWorkGroupInvocations;
    const auto maxSize        = ll::vec3ui {
        m_physicalDeviceLimits.maxComputeWorkGroupSize[0],
        m_physicalDeviceLimits.maxComputeWorkGroupSize[1],
        m_physicalDeviceLimits.maxComputeWorkGroupSize[2]};

    m_localComputeShapeD1 = computeOptimalLocalShape(ll::ComputeDimension::D1, maxInvocations, maxSize);
    m_localComputeShapeD2 = computeOptimalLocalShape(ll::ComputeDimension::D2, maxInvocations, maxSize);
    m_localComputeShapeD3 = computeOptimalLocalShape(ll::ComputeDimension::D3, maxInvocations, maxSize);
}

Device::~Device()
{
    m_device.destroyCommandPool(m_commandPool);
    m_device.destroy();
}

vk::Device& Device::get() noexcept
{
    return m_device;
}

vk::PhysicalDevice& Device::getPhysicalDevice() noexcept
{
    return m_physicalDevice;
}

const vk::PhysicalDeviceLimits& Device::getPhysicalDeviceLimits() noexcept
{
    return m_physicalDeviceLimits;
}

vk::CommandPool& Device::getCommandPool() noexcept
{
    return m_commandPool;
}

uint32_t Device::getComputeFamilyQueueIndex() const noexcept
{
    return m_computeQueueFamilyIndex;
}

ll::vec3ui Device::getComputeLocalShape(ll::ComputeDimension dimension) const noexcept
{
    switch (dimension) {
    case ll::ComputeDimension::D1:
        return m_localComputeShapeD1;
    case ll::ComputeDimension::D2:
        return m_localComputeShapeD2;
    case ll::ComputeDimension::D3:
        return m_localComputeShapeD3;
    }
}

bool Device::isImageDescriptorSupported(const ll::ImageDescriptor& descriptor) const noexcept
{

    auto formatProperties = vk::ImageFormatProperties {};

    auto result = m_physicalDevice.getImageFormatProperties(descriptor.getFormat(),
        descriptor.getImageType(),
        ll::impl::toVkImageTiling(descriptor.getTiling()),
        ll::impl::toVkImageUsageFlags(descriptor.getUsageFlags()),
        vk::ImageCreateFlags {},
        &formatProperties);

    if (result != vk::Result::eSuccess) {
        return false;
    }

    // check extend
    return !(descriptor.getWidth() > formatProperties.maxExtent.width || descriptor.getHeight() > formatProperties.maxExtent.height || descriptor.getDepth() > formatProperties.maxExtent.depth);
}

std::unique_ptr<ll::CommandBuffer> Device::createCommandBuffer()
{
    return std::make_unique<ll::CommandBuffer>(shared_from_this());
}

void Device::run(const ll::CommandBuffer& cmdBuffer)
{

    vk::SubmitInfo submitInfo = vk::SubmitInfo()
                                    .setCommandBufferCount(1)
                                    .setPCommandBuffers(&cmdBuffer.getVkCommandBuffer());

    auto result = m_queue.submit(1, &submitInfo, nullptr);

    ll::throwSystemErrorIf(result != vk::Result::eSuccess,
        ll::ErrorCode::VulkanError,
        "error submitting command buffer for execution.");

    m_queue.waitIdle();
}

} // namespace ll::lluvia

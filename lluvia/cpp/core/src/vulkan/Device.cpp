#include "lluvia/core/vulkan/Device.h"

#include "lluvia/core/error.h"
#include "lluvia/core/ImageDescriptor.h"
#include "lluvia/core/CommandBuffer.h"

namespace ll::vulkan {

Device::Device(const vk::Device& device,
               const vk::PhysicalDevice& physicalDevice,
               const uint32_t computeQueueFamilyIndex,
               const std::shared_ptr<ll::vulkan::Instance>& instance):
    m_device {device},
    m_physicalDevice {physicalDevice},
    m_computeQueueFamilyIndex {computeQueueFamilyIndex},
    m_instance {instance} {

    m_computeQueueFamilyIndex = getComputeFamilyQueueIndex();

    const auto createInfo = vk::CommandPoolCreateInfo()
                                .setQueueFamilyIndex(m_computeQueueFamilyIndex);

    m_commandPool = m_device.createCommandPool(createInfo);
    m_queue = m_device.getQueue(m_computeQueueFamilyIndex, 0);
}


Device::~Device() {
    m_device.destroyCommandPool(m_commandPool);
    m_device.destroy();
}


vk::Device& Device::get() noexcept {
    return m_device;
}


vk::PhysicalDevice& Device::getPhysicalDevice() noexcept {
    return m_physicalDevice;
}


vk::CommandPool& Device::getCommandPool() noexcept {
    return m_commandPool;
}


uint32_t Device::getComputeFamilyQueueIndex() const noexcept {
    return m_computeQueueFamilyIndex;
}

bool Device::isImageDescriptorSupported(const ll::ImageDescriptor &descriptor) const noexcept {

    auto formatProperties = vk::ImageFormatProperties {};

    auto result = m_physicalDevice.getImageFormatProperties(descriptor.getFormat(),
        descriptor.getImageType(), 
        descriptor.getTiling(), 
        descriptor.getUsageFlags(),
        vk::ImageCreateFlags {},
        &formatProperties);

    if (result != vk::Result::eSuccess) {
        return false;
    }

    // check extend
    return !(descriptor.getWidth() > formatProperties.maxExtent.width ||
             descriptor.getHeight() > formatProperties.maxExtent.height ||
             descriptor.getDepth() > formatProperties.maxExtent.depth);

}


std::unique_ptr<ll::CommandBuffer> Device::createCommandBuffer() {
    return std::make_unique<ll::CommandBuffer>(shared_from_this());
}


void Device::run(const ll::CommandBuffer& cmdBuffer) {

    vk::SubmitInfo submitInfo = vk::SubmitInfo()
        .setCommandBufferCount(1)
        .setPCommandBuffers(&cmdBuffer.getVkCommandBuffer());

    m_queue.submit(1, &submitInfo, nullptr);
    m_queue.waitIdle();
}

} // namespace ll::lluvia

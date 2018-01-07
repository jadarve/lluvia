#include "lluvia/core/impl/OneTimeSubmitCommandBuffer.h"


#include <vector>
#include <array>

namespace ll {
namespace impl {


OneTimeSubmitCommandBuffer::OneTimeSubmitCommandBuffer(
        const vk::Device& device,
        uint32_t queueFamilyIndex) :
    device {device} {

    vk::CommandPoolCreateInfo createInfo = vk::CommandPoolCreateInfo()
        .setQueueFamilyIndex(queueFamilyIndex);

    commandPool = device.createCommandPool(createInfo);

    vk::CommandBufferAllocateInfo allocInfo = vk::CommandBufferAllocateInfo()
        .setCommandPool(commandPool)
        .setCommandBufferCount(1);


    device.allocateCommandBuffers(&allocInfo, &commandBuffer);

    vk::CommandBufferBeginInfo beginInfo = vk::CommandBufferBeginInfo()
        .setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    // the command buffer is ready to record the commands
    commandBuffer.begin(beginInfo);
}

OneTimeSubmitCommandBuffer::~OneTimeSubmitCommandBuffer() {

    device.freeCommandBuffers(commandPool, 1, &commandBuffer);
    device.destroyCommandPool(commandPool, nullptr);
}

void OneTimeSubmitCommandBuffer::runAndWait(const vk::Queue& queue) {

    commandBuffer.end();

    vk::SubmitInfo submitInfo = vk::SubmitInfo()
        .setCommandBufferCount(1)
        .setPCommandBuffers(&commandBuffer);

    queue.submit(1, &submitInfo, nullptr);
    queue.waitIdle();
}

} // namespace impl
} // namespace ll

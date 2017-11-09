#include "lluvia/core/CommandBuffer.h"

namespace ll {

CommandBuffer::CommandBuffer(const vk::Device& device, const vk::CommandPool& cmdPool):
    device {device},
    commandPool {cmdPool} {

    const auto allocInfo = vk::CommandBufferAllocateInfo()
                            .setCommandPool(commandPool)
                            .setCommandBufferCount(1);

    auto cmdBuffers = device.allocateCommandBuffers(allocInfo);
    commandBuffer = cmdBuffers[0];
}


CommandBuffer::~CommandBuffer() {

    device.freeCommandBuffers(commandPool, 1, &commandBuffer);
}


void CommandBuffer::begin() {

    vk::CommandBufferBeginInfo beginInfo = vk::CommandBufferBeginInfo()
        .setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse);

    commandBuffer.begin(beginInfo);
}


void CommandBuffer::end() {
    commandBuffer.end();
}

} // namespace ll
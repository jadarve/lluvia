#include "lluvia/core/CommandBuffer.h"

#include "lluvia/core/Buffer.h"
#include "lluvia/core/ComputeNode.h"
#include "lluvia/core/Image.h"

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


void CommandBuffer::run(const ll::ComputeNode& node) {

    node.record(commandBuffer);
}


void CommandBuffer::copyBuffer(const ll::Buffer& src, const ll::Buffer& dst) {

    if (dst.getSize() < src.getSize()) {
        throw std::system_error(createErrorCode(ll::ErrorCode::BufferCopyError), "destination size must be greater or equal than source: got " + std::to_string(dst.getSize()) + " expected: " + std::to_string(src.getSize()));
    }
    

    auto copyInfo = vk::BufferCopy()
        .setSrcOffset(0)
        .setDstOffset(0)
        .setSize(src.getSize());

    commandBuffer.copyBuffer(src.vkBuffer, dst.vkBuffer, 1, &copyInfo);
}


void CommandBuffer::copyBufferToImage(const ll::Buffer& src, const ll::Image& dst) {

    auto imgSubresourceLayers = vk::ImageSubresourceLayers {}
        .setAspectMask(vk::ImageAspectFlagBits::eColor)
        .setMipLevel(0)
        .setBaseArrayLayer(0)
        .setLayerCount(1);

    auto copyInfo = vk::BufferImageCopy {}
        .setBufferOffset(0)
        .setBufferImageHeight(0) // thightly packed
        .setBufferRowLength(0)
        .setImageSubresource(imgSubresourceLayers)
        .setImageOffset({0, 0, 0})
        .setImageExtent({dst.getWidth(), dst.getHeight(), dst.getDepth()});

    commandBuffer.copyBufferToImage(src.vkBuffer, dst.vkImage, dst.vkLayout, 1, &copyInfo);
}


void CommandBuffer::copyImageToBuffer(const ll::Image& src, const ll::Buffer& dst) {

    auto imgSubresourceLayers = vk::ImageSubresourceLayers {}
        .setAspectMask(vk::ImageAspectFlagBits::eColor)
        .setMipLevel(0)
        .setBaseArrayLayer(0)
        .setLayerCount(1);

    auto copyInfo = vk::BufferImageCopy {}
        .setBufferOffset(0)
        .setBufferImageHeight(0) // thightly packed
        .setBufferRowLength(0)
        .setImageSubresource(imgSubresourceLayers)
        .setImageOffset({0, 0, 0})
        .setImageExtent({src.getWidth(), src.getHeight(), src.getDepth()});

    commandBuffer.copyImageToBuffer(src.vkImage, src.vkLayout, dst.vkBuffer, 1, &copyInfo);
}


void CommandBuffer::changeImageLayout(ll::Image& image, const vk::ImageLayout newLayout) {

    auto barrier = vk::ImageMemoryBarrier {}
                    .setOldLayout(image.vkLayout)
                    .setNewLayout(newLayout)
                    .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                    .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                    .setImage(image.vkImage)
                    .setSrcAccessMask(vk::AccessFlagBits::eMemoryRead)      // TODO ???
                    .setDstAccessMask(vk::AccessFlagBits::eMemoryWrite);    // TODO ???

    barrier.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
    barrier.subresourceRange.setBaseMipLevel(0);
    barrier.subresourceRange.setLevelCount(1);
    barrier.subresourceRange.setBaseArrayLayer(0);
    barrier.subresourceRange.setLayerCount(1);


    commandBuffer.pipelineBarrier(
        vk::PipelineStageFlagBits::eComputeShader, vk::PipelineStageFlagBits::eComputeShader,
        vk::DependencyFlagBits::eDeviceGroupKHX,
        0, nullptr,
        0, nullptr,
        1, &barrier);

    // FIXME: this should be set only after the pipelineBarrier is executed
    image.vkLayout = newLayout;
}


void CommandBuffer::memoryBarrier() {

    auto barrier = vk::MemoryBarrier {}
                    .setSrcAccessMask(vk::AccessFlagBits::eShaderWrite)
                    .setDstAccessMask(vk::AccessFlagBits::eShaderRead);


    commandBuffer.pipelineBarrier(
        vk::PipelineStageFlagBits::eComputeShader, vk::PipelineStageFlagBits::eComputeShader,
        vk::DependencyFlagBits::eDeviceGroupKHX,
        1, &barrier,
        0, nullptr,
        0, nullptr);
}

} // namespace ll
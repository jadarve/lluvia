#include "lluvia/core/CommandBuffer.h"

#include "lluvia/core/Buffer.h"
#include "lluvia/core/ComputeNode.h"
#include "lluvia/core/Duration.h"
#include "lluvia/core/Image.h"
#include "lluvia/core/ImageView.h"

#include "lluvia/core/vulkan/Device.h"

namespace ll {

CommandBuffer::CommandBuffer(const std::shared_ptr<ll::vulkan::Device> &device):
    m_device{device} {

    const auto allocInfo = vk::CommandBufferAllocateInfo()
                            .setCommandPool(m_device->getCommandPool())
                            .setCommandBufferCount(1);

    auto cmdBuffers = m_device->get().allocateCommandBuffers(allocInfo);
    m_commandBuffer = cmdBuffers[0];
}

CommandBuffer::~CommandBuffer() {
    m_device->get().freeCommandBuffers(m_device->getCommandPool(), 1, &m_commandBuffer);
}


const vk::CommandBuffer& CommandBuffer::getVkCommandBuffer() const noexcept {
    return m_commandBuffer;
}


void CommandBuffer::begin() {

    vk::CommandBufferBeginInfo beginInfo = vk::CommandBufferBeginInfo()
        .setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse);

    m_commandBuffer.begin(beginInfo);
}


void CommandBuffer::end() {
    m_commandBuffer.end();
}


void CommandBuffer::run(const ll::ComputeNode& node) {

    node.record(*this);
}


void CommandBuffer::copyBuffer(const ll::Buffer& src, const ll::Buffer& dst) {

    if (dst.getSize() < src.getSize()) {
        throw std::system_error(createErrorCode(ll::ErrorCode::BufferCopyError), "destination size must be greater or equal than source: got " + std::to_string(dst.getSize()) + " expected: " + std::to_string(src.getSize()));
    }
    

    auto copyInfo = vk::BufferCopy()
        .setSrcOffset(0)
        .setDstOffset(0)
        .setSize(src.getSize());

    m_commandBuffer.copyBuffer(src.m_vkBuffer, dst.m_vkBuffer, 1, &copyInfo);
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

    m_commandBuffer.copyBufferToImage(src.m_vkBuffer, dst.m_vkImage, dst.m_vkLayout, 1, &copyInfo);
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

    m_commandBuffer.copyImageToBuffer(src.m_vkImage, src.m_vkLayout, dst.m_vkBuffer, 1, &copyInfo);
}

void CommandBuffer::copyImageToImage(const ll::Image &src, const ll::Image &dst) {

    auto imgSubresourceLayers = vk::ImageSubresourceLayers{}
                                    .setAspectMask(vk::ImageAspectFlagBits::eColor)
                                    .setMipLevel(0)
                                    .setBaseArrayLayer(0)
                                    .setLayerCount(1);

    auto copyRegion = vk::ImageCopy {}
        .setSrcOffset({0, 0, 0})
        .setSrcSubresource(imgSubresourceLayers)
        .setDstOffset({0, 0, 0})
        .setDstSubresource(imgSubresourceLayers)
        .setExtent({src.getWidth(), src.getHeight(), src.getDepth()});

    m_commandBuffer.copyImage(src.m_vkImage,
        src.m_vkLayout,
        dst.m_vkImage,
        dst.m_vkLayout,
        1,
        &copyRegion);
}

void CommandBuffer::changeImageLayout(ll::Image& image, const vk::ImageLayout newLayout) {

    auto barrier = vk::ImageMemoryBarrier {}
                    .setOldLayout(image.m_vkLayout)
                    .setNewLayout(newLayout)
                    .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                    .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                    .setImage(image.m_vkImage)
                    .setSrcAccessMask(vk::AccessFlagBits::eMemoryRead)      // TODO ???
                    .setDstAccessMask(vk::AccessFlagBits::eMemoryWrite);    // TODO ???

    barrier.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
    barrier.subresourceRange.setBaseMipLevel(0);
    barrier.subresourceRange.setLevelCount(1);
    barrier.subresourceRange.setBaseArrayLayer(0);
    barrier.subresourceRange.setLayerCount(1);


    m_commandBuffer.pipelineBarrier(
        vk::PipelineStageFlagBits::eComputeShader, vk::PipelineStageFlagBits::eComputeShader,
        vk::DependencyFlagBits::eDeviceGroupKHR,
        0, nullptr,
        0, nullptr,
        1, &barrier);

    // FIXME: this should be set only after the pipelineBarrier is executed
    image.m_vkLayout = newLayout;
}

void CommandBuffer::changeImageLayout(ll::ImageView& imageView, const vk::ImageLayout newLayout) {

    this->changeImageLayout(*(imageView.getImage()), newLayout);
}


void CommandBuffer::memoryBarrier() {

    auto barrier = vk::MemoryBarrier {}
                    .setSrcAccessMask(vk::AccessFlagBits::eShaderWrite)
                    .setDstAccessMask(vk::AccessFlagBits::eShaderRead);


    m_commandBuffer.pipelineBarrier(
        vk::PipelineStageFlagBits::eComputeShader, vk::PipelineStageFlagBits::eComputeShader,
        vk::DependencyFlagBits::eDeviceGroupKHR,
        1, &barrier,
        0, nullptr,
        0, nullptr);
}


void CommandBuffer::clearImage(ll::Image &image) {

    auto clearColor = vk::ClearColorValue{std::array<int32_t, 4>{0, 0, 0, 0}};

    auto range = vk::ImageSubresourceRange()
                    .setAspectMask(vk::ImageAspectFlagBits::eColor)
                    .setBaseMipLevel(0)
                    .setLevelCount(1)
                    .setBaseArrayLayer(0)
                    .setLayerCount(1);

    m_commandBuffer.clearColorImage(image.m_vkImage, image.m_vkLayout, clearColor, range);
}

void CommandBuffer::durationStart(ll::Duration &duration) {

    m_commandBuffer.resetQueryPool(duration.getQueryPool(), 0, 2);

    m_commandBuffer.writeTimestamp(vk::PipelineStageFlagBits::eBottomOfPipe,
                                   duration.getQueryPool(),
                                   duration.getStartTimeQueryIndex());
}

void CommandBuffer::durationEnd(ll::Duration &duration) {

    m_commandBuffer.writeTimestamp(vk::PipelineStageFlagBits::eBottomOfPipe,
                                   duration.getQueryPool(),
                                   duration.getEndTimeQueryIndex());
}

} // namespace ll
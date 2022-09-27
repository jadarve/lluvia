/**
@file       Image.cpp
@brief      Image class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#include "lluvia/core/image/Image.h"

#include "lluvia/core/CommandBuffer.h"
#include "lluvia/core/image/ImageView.h"
#include "lluvia/core/image/ImageViewDescriptor.h"
#include "lluvia/core/memory/Memory.h"
#include "lluvia/core/Session.h"

#include "lluvia/core/vulkan/Device.h"

namespace ll {


Image::Image(
    const std::shared_ptr<ll::vulkan::Device>& device,
    const vk::Image& vkImage,
    const ll::ImageDescriptor& descriptor,
    const std::shared_ptr<ll::Memory>& memory,
    const ll::MemoryAllocationInfo& allocInfo,
    const ll::ImageLayout layout) :

    m_device       {device},
    m_descriptor   {descriptor},
    m_allocInfo    (allocInfo),
    m_vkImage      {vkImage},
    m_layout       {layout},
    m_memory       {memory} {

}


Image::~Image() {

    m_memory->releaseImage(*this);
}


ll::ObjectType Image::getType() const noexcept {
    return ObjectType::Image;
}


ll::MemoryAllocationInfo Image::getAllocationInfo() const noexcept {
    return m_allocInfo;
}


const std::shared_ptr<ll::Memory>& Image::getMemory() const noexcept {
    return m_memory;
}


uint64_t Image::getSize() const noexcept {
    return m_allocInfo.size;
}


uint64_t Image::getMinimumSize() const noexcept {
    return m_descriptor.getSize();
}


const ll::ImageDescriptor& Image::getDescriptor() const noexcept {
    return m_descriptor;
}


ll::ImageUsageFlags Image::getUsageFlags() const noexcept {
    return m_descriptor.getUsageFlags();
}


uint32_t Image::getUsageFlagsUnsafe() const noexcept {
    return m_descriptor.getUsageFlagsUnsafe();
}


ll::ImageLayout Image::getLayout() const noexcept {
    return m_layout;
}


ll::ImageTiling Image::getTiling() const noexcept {
    return m_descriptor.getTiling();
}


ll::ChannelType Image::getChannelType() const noexcept {
    return m_descriptor.getChannelType();
}


uint64_t Image::getChannelTypeSize() const noexcept {
    return ll::getChannelTypeSize(m_descriptor.getChannelType());
}


uint32_t Image::getWidth() const noexcept {
    return m_descriptor.getWidth();
}


uint32_t Image::getHeight() const noexcept {
    return m_descriptor.getHeight();
}


uint32_t Image::getDepth() const noexcept {
    return m_descriptor.getDepth();
}

ll::vec3ui Image::getShape() const noexcept {
    return m_descriptor.getShape();
}

std::shared_ptr<ll::ImageView> Image::createImageView(const ll::ImageViewDescriptor& tDescriptor) {
    return std::shared_ptr<ll::ImageView> {new ll::ImageView {m_device, shared_from_this(), tDescriptor}};
}

void Image::changeImageLayout(const ll::ImageLayout newLayout) {

    auto cmdBuffer = m_device->createCommandBuffer();

    cmdBuffer->begin();
    cmdBuffer->changeImageLayout(*this, newLayout);
    cmdBuffer->end();
    m_device->run(*cmdBuffer);

}

void Image::clear() {

    auto cmdBuffer = m_device->createCommandBuffer();

    cmdBuffer->begin();
    cmdBuffer->clearImage(*this);
    cmdBuffer->end();
    m_device->run(*cmdBuffer);
}

void Image::copyTo(ll::Image& dst) {

    const auto srcCurrentLayout = getLayout();
    const auto dstCurrentLayout = dst.getLayout();

    auto cmdBuffer = m_device->createCommandBuffer();

    cmdBuffer->begin();
    cmdBuffer->changeImageLayout(*this, ll::ImageLayout::TransferSrcOptimal);
    cmdBuffer->changeImageLayout(dst, ll::ImageLayout::TransferDstOptimal);
    cmdBuffer->copyImageToImage(*this, dst);
    cmdBuffer->changeImageLayout(*this, srcCurrentLayout);
    cmdBuffer->changeImageLayout(dst, dstCurrentLayout);
    cmdBuffer->end();
    
    m_device->run(*cmdBuffer);
}

} // namespace ll

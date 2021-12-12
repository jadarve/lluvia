/**
@file       Image.cpp
@brief      Image class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#include "lluvia/core/Image.h"

#include "lluvia/core/CommandBuffer.h"
#include "lluvia/core/ImageView.h"
#include "lluvia/core/ImageViewDescriptor.h"
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
    const vk::ImageLayout layout) :

    m_device       {device},
    m_descriptor   {descriptor},
    m_allocInfo    (allocInfo),
    m_vkImage      {vkImage},
    m_vkLayout     {layout},
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


const ll::ImageDescriptor& Image::getDescriptor() const noexcept {
    return m_descriptor;
}


vk::ImageUsageFlags Image::getUsageFlags() const noexcept {
    return m_descriptor.getUsageFlags();
}


uint32_t Image::getUsageFlagsUnsafe() const noexcept {
    return m_descriptor.getUsageFlagsUnsafe();
}


vk::ImageLayout Image::getLayout() const noexcept {
    return m_vkLayout;
}


vk::ImageTiling Image::getTiling() const noexcept {
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

void Image::changeImageLayout(const vk::ImageLayout newLayout) {

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

} // namespace ll

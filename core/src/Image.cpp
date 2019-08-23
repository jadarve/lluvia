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
#include "lluvia/core/Memory.h"
#include "lluvia/core/Session.h"

namespace ll {


Image::Image(
    const vk::Device& tDevice,
    const vk::Image& tVkImage,
    const ll::ImageDescriptor& tDescriptor,
    const std::shared_ptr<ll::Memory>& tMemory,
    const ll::MemoryAllocationInfo& tAllocInfo,
    const vk::ImageLayout tLayout) :

    m_descriptor   {tDescriptor},
    m_allocInfo    (tAllocInfo),
    m_device       {tDevice},
    m_vkImage      {tVkImage},
    m_vkLayout     {tLayout},
    m_memory       {tMemory} {

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


vk::ImageUsageFlags Image::getUsageFlags()const noexcept {
    return m_descriptor.getUsageFlags();
}


vk::ImageLayout Image::getLayout() const noexcept {
    return m_vkLayout;
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

    const auto& session = m_memory->getSession();

    auto cmdBuffer = session->createCommandBuffer();

    cmdBuffer->begin();
    cmdBuffer->changeImageLayout(*this, newLayout);
    cmdBuffer->end();
    session->run(*cmdBuffer);

}

} // namespace ll

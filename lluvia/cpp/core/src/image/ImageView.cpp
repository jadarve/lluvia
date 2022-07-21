/**
@file       ImageView.cpp
@brief      ImageView class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#include "lluvia/core/image/ImageView.h"

#include "lluvia/core/image/Image.h"
#include "lluvia/core/image/ImageViewDescriptor.h"
#include "lluvia/core/memory/Memory.h"

#include "lluvia/core/vulkan/Device.h"


namespace ll {

ImageView::ImageView(const std::shared_ptr<ll::vulkan::Device>& device, 
                     const std::shared_ptr<ll::Image>& image,
                     const ll::ImageViewDescriptor& descriptor) :
    m_descriptor {descriptor},
    m_device {device},
    m_image  {image} {

    auto imageViewInfo = vk::ImageViewCreateInfo {}
                            .setViewType(vk::ImageViewType::e2D)        // TODO: set according to image extend
                            .setFormat(m_image->getDescriptor().getFormat())
                            .setImage(m_image->m_vkImage);

    // TODO
    imageViewInfo.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
    imageViewInfo.subresourceRange.setBaseMipLevel(0);
    imageViewInfo.subresourceRange.setLevelCount(1);
    imageViewInfo.subresourceRange.setBaseArrayLayer(0);
    imageViewInfo.subresourceRange.setLayerCount(1);

    
    m_vkImageView = m_device->get().createImageView(imageViewInfo);

    if (m_descriptor.isSampled()) {
        m_vkSampler = m_device->get().createSampler(m_descriptor.getVkSamplerCreateInfo());
    }
}


ImageView::~ImageView() {

    m_device->get().destroyImageView(m_vkImageView);

    if (m_descriptor.isSampled()) {
        m_device->get().destroySampler(m_vkSampler);
    }
}


ll::ObjectType ImageView::getType() const noexcept {
    return ll::ObjectType::ImageView;
}


const std::shared_ptr<ll::Image>& ImageView::getImage() const noexcept {
    return m_image;
}


const std::shared_ptr<ll::Memory>& ImageView::getMemory() const noexcept {
    return m_image->getMemory();
}


ll::MemoryAllocationInfo ImageView::getAllocationInfo() const noexcept {
    return m_image->getAllocationInfo();
}


uint64_t ImageView::getSize() const noexcept {
    return m_image->getSize();
}


const ll::ImageDescriptor& ImageView::getImageDescriptor() const noexcept {
    return m_image->getDescriptor();
}


ll::ImageUsageFlags ImageView::getUsageFlags() const noexcept {
    return m_image->getUsageFlags();
}


uint32_t ImageView::getUsageFlagsUnsafe() const noexcept {
    return m_image->getUsageFlagsUnsafe();
}


ll::ImageLayout ImageView::getLayout() const noexcept {
    return m_image->getLayout();
}


ll::ImageTiling ImageView::getTiling() const noexcept {
    return m_image->getTiling();
}


ll::ChannelType ImageView::getChannelType() const noexcept {
    return m_image->getChannelType();
}


uint64_t ImageView::getChannelTypeSize() const noexcept {
    return m_image->getChannelTypeSize();
}


uint32_t ImageView::getWidth() const noexcept {
    return m_image->getWidth();
}


uint32_t ImageView::getHeight() const noexcept {
    return m_image->getHeight();
}


uint32_t ImageView::getDepth() const noexcept {
    return m_image->getDepth();
}


ll::vec3ui ImageView::getShape() const noexcept {
    return m_image->getShape();
}


const ll::ImageViewDescriptor& ImageView::getDescriptor() const noexcept {
    return m_descriptor;
}


void ImageView::changeImageLayout(const ll::ImageLayout newLayout) {
    m_image->changeImageLayout(newLayout);
}


void ImageView::clear() {
    m_image->clear();
}

void ImageView::copyTo(ll::ImageView& dst) {
    m_image->copyTo(*dst.m_image);
}

} // namespace ll

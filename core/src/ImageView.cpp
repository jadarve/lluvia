/**
@file       ImageView.cpp
@brief      ImageView class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#include "lluvia/core/ImageView.h"

#include "lluvia/core/Image.h"
#include "lluvia/core/ImageViewDescriptor.h"
#include "lluvia/core/Memory.h"


namespace ll {

ImageView::ImageView(vk::Device tDevice, 
                     const std::shared_ptr<ll::Image>& tImage,
                     const ll::ImageViewDescriptor& tDescriptor) :
    m_descriptor {tDescriptor},
    m_device {tDevice},
    m_image  {tImage} {

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

    
    m_vkImageView = m_device.createImageView(imageViewInfo);

    if (m_descriptor.isSampled()) {
        m_vkSampler = m_device.createSampler(m_descriptor.getVkSamplerCreateInfo());
    }
}


ImageView::~ImageView() {

    m_device.destroyImageView(m_vkImageView);

    if (m_descriptor.isSampled()) {
        m_device.destroySampler(m_vkSampler);
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


const std::shared_ptr<ll::Session>& ImageView::getSession() const noexcept {
    return m_image->getSession();
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


vk::ImageUsageFlags ImageView::getUsageFlags() const noexcept {
    return m_image->getUsageFlags();
}


vk::ImageLayout ImageView::getLayout() const noexcept {
    return m_image->getLayout();
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


void ImageView::changeImageLayout(const vk::ImageLayout newLayout) {
    m_image->changeImageLayout(newLayout);
}

} // namespace ll

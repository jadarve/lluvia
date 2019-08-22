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
                     std::shared_ptr<ll::Image> tImage,
                     const ll::ImageViewDescriptor& tDescriptor) :
    descriptor {tDescriptor},
    device {tDevice},
    image  {tImage} {

    auto imageViewInfo = vk::ImageViewCreateInfo {}
                            .setViewType(vk::ImageViewType::e2D)        // TODO: set according to image extend
                            .setFormat(image->descriptor.getFormat())
                            .setImage(image->vkImage);

    // TODO
    imageViewInfo.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
    imageViewInfo.subresourceRange.setBaseMipLevel(0);
    imageViewInfo.subresourceRange.setLevelCount(1);
    imageViewInfo.subresourceRange.setBaseArrayLayer(0);
    imageViewInfo.subresourceRange.setLayerCount(1);

    
    vkImageView = device.createImageView(imageViewInfo);

    if (descriptor.isSampled()) {
        vkSampler = device.createSampler(descriptor.getVkSamplerCreateInfo());
    }
}


ImageView::~ImageView() {

    device.destroyImageView(vkImageView);

    if (descriptor.isSampled()) {
        device.destroySampler(vkSampler);
    }
}


ll::ObjectType ImageView::getType() const noexcept {
    return ll::ObjectType::ImageView;
}


std::shared_ptr<ll::Image> ImageView::getImage() const noexcept {
    return image;
}


const ll::ImageDescriptor& ImageView::getImageDescriptor() const noexcept {
    return image->getDescriptor();
}

} // namespace ll

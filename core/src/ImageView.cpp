#include "lluvia/core/ImageView.h"

#include "lluvia/core/Image.h"
#include "lluvia/core/ImageViewDescriptor.h"
#include "lluvia/core/Memory.h"


namespace ll {

ll::ObjectType ImageView::getType() const noexcept {
    return ll::ObjectType::ImageView;
}

ImageView::ImageView(vk::Device device, 
                     std::shared_ptr<ll::Image> image,
                     const ll::ImageViewDescriptor& descriptor) :
    device {device},
    image  {image} {

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

    vkSampler = device.createSampler(descriptor.getVkSamplerCreateInfo());
}


ImageView::~ImageView() {

    device.destroyImageView(vkImageView);
    device.destroySampler(vkSampler);
}

} // namespace ll
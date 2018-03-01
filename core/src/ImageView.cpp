#include "lluvia/core/ImageView.h"

#include "lluvia/core/Image.h"
#include "lluvia/core/ImageViewDescriptor.h"
#include "lluvia/core/Memory.h"


namespace ll {

ImageView::ImageView(vk::Device device, 
                     std::shared_ptr<ll::Image> image,
                     const ll::ImageViewDescriptor& descriptor) :
    device {device},
    image  {std::move(image)} {

    auto imageViewInfo = vk::ImageViewCreateInfo {}
                            .setViewType(vk::ImageViewType::e2D)        // TODO: set according to image extend
                            .setFormat(image->descriptor.getFormat())
                            .setImage(image->image);

    // TODO
    imageViewInfo.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
    imageViewInfo.subresourceRange.setBaseMipLevel(0);
    imageViewInfo.subresourceRange.setLevelCount(1);
    imageViewInfo.subresourceRange.setBaseArrayLayer(0);
    imageViewInfo.subresourceRange.setLayerCount(1);

    
    imageView = device.createImageView(imageViewInfo);

    sampler = device.createSampler(descriptor.getVkSamplerCreateInfo());
}


ImageView::~ImageView() {

    device.destroyImageView(imageView);
    device.destroySampler(sampler);
}

} // namespace ll
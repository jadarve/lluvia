/**
@file       Image.cpp
@brief      Image class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#include "lluvia/core/Image.h"
#include "lluvia/core/ImageView.h"
#include "lluvia/core/ImageViewDescriptor.h"

#include "lluvia/core/Memory.h"

namespace ll {


Image::Image( const vk::Device& device, const vk::Image& vkImage, const ll::ImageDescriptor& descriptor,
              const std::shared_ptr<ll::Memory>& memory, const ll::MemoryAllocationInfo& allocInfo,
              const vk::ImageLayout layout) :

    descriptor   {descriptor},
    allocInfo    (allocInfo),
    device       {device},
    vkImage      {vkImage},
    vkLayout     {layout},
    memory       {memory} {

}


Image::~Image() {

    memory->releaseImage(*this);
}


ll::ObjectType Image::getType() const noexcept {
    return ObjectType::Image;
}


ll::MemoryAllocationInfo Image::getAllocationInfo() const noexcept {
    return allocInfo;
}


uint64_t Image::getSize() const noexcept {
    return allocInfo.size;
}


vk::ImageUsageFlags Image::getUsageFlags()const noexcept {
    return descriptor.getUsageFlags();
}


vk::ImageLayout Image::getLayout() const noexcept {
    return vkLayout;
}


ll::ChannelType Image::getChannelType() const noexcept {
    return descriptor.getChannelType();
}


uint64_t Image::getChannelTypeSize() const noexcept {
    return ll::getChannelTypeSize(descriptor.getChannelType());
}


uint32_t Image::getChannelCount() const noexcept {
    return descriptor.getChannelCount();
}


uint32_t Image::getWidth() const noexcept {
    return descriptor.getWidth();
}


uint32_t Image::getHeight() const noexcept {
    return descriptor.getHeight();
}


uint32_t Image::getDepth() const noexcept {
    return descriptor.getDepth();
}

ll::vec3ui Image::getShape() const noexcept {
    return descriptor.getShape();
}

std::shared_ptr<ll::ImageView> Image::createImageView(const ll::ImageViewDescriptor& descriptor) {

    return std::shared_ptr<ll::ImageView> {new ll::ImageView {device, shared_from_this(), descriptor}};
}

} // namespace ll

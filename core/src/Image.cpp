#include "lluvia/core/Image.h"
#include "lluvia/core/ImageView.h"
#include "lluvia/core/ImageViewDescriptor.h"

#include "lluvia/core/Memory.h"

namespace ll {


Image::Image( const vk::Image& vkImage, const ll::ImageDescriptor& descriptor,
              std::shared_ptr<ll::Memory> memory, const ll::MemoryAllocationInfo& allocInfo,
              const vk::ImageLayout layout) :

    descriptor {descriptor},
    allocInfo  (allocInfo),
    image      {vkImage},
    layout     {layout},
    memory     {memory} {

}


Image::~Image() {

    memory->releaseImage(*this);
}


ObjectType Image::getType() const noexcept {
    return ObjectType::Image;
}


std::shared_ptr<ll::ImageView> Image::createImageView(const ll::ImageViewDescriptor& descriptor) const {

    return std::shared_ptr<ll::ImageView> {};
}

} // namespace ll

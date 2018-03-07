#ifndef LLUVIA_CORE_IMAGE_H_
#define LLUVIA_CORE_IMAGE_H_

#include "lluvia/core/ImageDescriptor.h"
#include "lluvia/core/MemoryAllocationInfo.h"
#include "lluvia/core/Object.h"


#include <array>
#include <cstdint>

#include <vulkan/vulkan.hpp>


namespace ll {

class ComputeNode;
class ImageView;
class ImageViewDescriptor;
class Memory;
class Session;


class Image: public Object,
             public std::enable_shared_from_this<ll::Image> {

public:
    Image()                = delete;
    Image(const Image&)    = delete;
    Image(Image&&)         = delete;

    ~Image();

    Image& operator = (const Image&) = delete;
    Image& operator = (Image&&)      = delete;

    ll::ObjectType getType() const noexcept override;

    std::shared_ptr<ll::ImageView> createImageView(const ll::ImageViewDescriptor& descriptor);

private:
    Image( const vk::Device& device, const vk::Image& vkImage, const ll::ImageDescriptor& descriptor,
           std::shared_ptr<ll::Memory> memory, const ll::MemoryAllocationInfo& allocInfo,
           const vk::ImageLayout layout);

    ll::ImageDescriptor descriptor;
    ll::MemoryAllocationInfo allocInfo;

    vk::Device      device;
    vk::Image       vkImage;
    vk::ImageLayout vkLayout;

    // Shared pointer to the memory this image was created from
    // This will keep the memory alive until this image is deleted
    // avoiding reference to a corrupted memory location.
    std::shared_ptr<ll::Memory> memory;

friend class ll::ComputeNode;
friend class ll::ImageView;
friend class ll::Memory;
friend class ll::Session;
};

} // namespace ll

#endif /* LLUVIA_CORE_IMAGE_H_ */

#ifndef LLUVIA_CORE_IMAGE_VIEW_H_
#define LLUVIA_CORE_IMAGE_VIEW_H_

#include "lluvia/core/Object.h"
#include "lluvia/core/ImageViewDescriptor.h"

#include <memory>

#include <vulkan/vulkan.hpp>


namespace ll {

class Image;


class ImageView : public Object {

public:
    ImageView()                    = delete;
    ImageView(const ImageView&)    = delete;
    ImageView(ImageView&&)         = delete;

    ~ImageView();

    ImageView& operator = (const ImageView&) = delete;
    ImageView& operator = (ImageView&&)      = delete;

    ll::ObjectType getType() const noexcept override;

    std::shared_ptr<ll::Image> getImage() const noexcept;

    ll::ImageFilterMode  getFilterMode()            const noexcept;
    ll::ImageAddressMode getAddressModeU()          const noexcept;
    ll::ImageAddressMode getAddressModeV()          const noexcept;
    ll::ImageAddressMode getAddressModeW()          const noexcept;
    bool                 getNormalizedCoordinates() const noexcept;
    bool                 isSampled()                const noexcept;

private:
    ImageView(vk::Device device, std::shared_ptr<ll::Image> image, const ll::ImageViewDescriptor& descriptor);
    
    ll::ImageViewDescriptor descriptor;

    vk::Device    device;
    vk::ImageView vkImageView;
    vk::Sampler   vkSampler;

    std::shared_ptr<ll::Image> image;


friend class Image;
friend class ComputeNode;
};

} // namespace ll

#endif /* LLUVIA_CORE_IMAGE_VIEW_H_ */

#ifndef LLUVIA_CORE_IMAGE_VIEW_H_
#define LLUVIA_CORE_IMAGE_VIEW_H_

#include <memory>

#include <vulkan/vulkan.hpp>


namespace ll {

class Image;
class ImageViewDescriptor;

class ImageView {

public:
    ImageView()                    = delete;
    ImageView(const ImageView&)    = delete;
    ImageView(ImageView&&)         = delete;

    ~ImageView();

    ImageView& operator = (const ImageView&) = delete;
    ImageView& operator = (ImageView&&)      = delete;

private:
    ImageView(vk::Device device, std::shared_ptr<ll::Image> image, const ll::ImageViewDescriptor& descriptor);
    
    vk::Device    device;
    vk::ImageView imageView;
    vk::Sampler   sampler;

    std::shared_ptr<ll::Image> image;


friend class Image;
};

} // namespace ll

#endif /* LLUVIA_CORE_IMAGE_VIEW_H_ */

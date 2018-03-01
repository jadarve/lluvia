#ifndef LLUVIA_CORE_IMAGE_VIEW_DESCRIPTOR_H_
#define LLUVIA_CORE_IMAGE_VIEW_DESCRIPTOR_H_

#include <cstdint>

#include <vulkan/vulkan.hpp>


namespace ll {

enum class ImageAxis : uint32_t {
    U = 0,
    V = 1,
    W = 2
};

enum class ImageFilterMode : uint32_t {
    Nearest,
    Linear
};


enum class ImageAddressMode : uint32_t {
    Repeat,
    MirroredRepeat,
    ClampToEdge,
    ClampToBorder,
    MirrorClampToEdge
};


class ImageViewDescriptor {

public:
    ImageViewDescriptor()                                                   = default;
    ImageViewDescriptor(const ImageViewDescriptor& descriptor)              = default;
    ImageViewDescriptor(ImageViewDescriptor&& descriptor)                   = default;

    ~ImageViewDescriptor()                                                  = default;
    
    ImageViewDescriptor& operator = (const ImageViewDescriptor& descriptor) = default;
    ImageViewDescriptor& operator = (ImageViewDescriptor&& descriptor)      = default;

    ImageViewDescriptor& setFilteringMode(ll::ImageFilterMode filterMode);
    ImageViewDescriptor& setAddressMode(ll::ImageAddressMode addressMode);
    ImageViewDescriptor& setAddressMode(ll::ImageAxis axis, ll::ImageAddressMode addressMode);
    ImageViewDescriptor& setNormalizedCoordinates(bool normalizedCoordinates);


    vk::SamplerCreateInfo getVkSamplerCreateInfo() const noexcept;

private:

    ll::ImageFilterMode filterMode;

    /**
     * Address mode for X, Y, Z axes
     */
    std::array<ll::ImageAddressMode, 3> addressMode;

    bool normalizedCoordinates;
};


} // namespace ll

#endif /* LLUVIA_CORE_IMAGE_VIEW_DESCRIPTOR_H_ */

#ifndef LLUVIA_CORE_IMAGE_VIEW_DESCRIPTOR_H_
#define LLUVIA_CORE_IMAGE_VIEW_DESCRIPTOR_H_

#include "lluvia/core/impl/enum_utils.h"

#include <cstdint>

#include <vulkan/vulkan.hpp>


namespace ll {

class ImageView;

enum class ImageAxis : uint32_t {
    U = 0,
    V = 1,
    W = 2
};

enum class ImageFilterMode : uint32_t {
    Nearest = 0,
    Linear  = 1
};


enum class ImageAddressMode : uint32_t {
    Repeat            = 0,
    MirroredRepeat    = 1,
    ClampToEdge       = 2,
    ClampToBorder     = 3,
    MirrorClampToEdge = 4
};


namespace impl {

    constexpr const std::array<const char*, 2> ImageFilterModeStrings {{
        "NEAREST",
        "LINEAR"
    }};


    constexpr const std::array<const char*, 5> ImageAddressModeStrings {{
        "REPEAT",
        "MIRRORED_REPEAT",
        "CLAMP_TO_EDGE",
        "CLAMP_T_OBORDER",
        "MIRROR_CLAMP_TO_EDGE"
    }};

} // namespace impl


template<typename T = std::string>
inline T imageFilterModeToString(ll::ImageFilterMode&& value) {
    return ll::impl::enumToString<ll::ImageFilterMode, impl::ImageFilterModeStrings.size(), impl::ImageFilterModeStrings>(std::forward<ll::ImageFilterMode>(value));
}


template<typename T>
inline ll::ImageFilterMode stringToImageFilterMode(T&& stringValue) {
    return impl::stringToEnum<ll::ImageFilterMode, T, ll::impl::ImageFilterModeStrings.size(), impl::ImageFilterModeStrings>(std::forward<T>(stringValue));
}


template<typename T = std::string>
inline T imageAddressModeToString(ll::ImageAddressMode&& value) {
    return impl::enumToString<ll::ImageAddressMode, ll::impl::ImageAddressModeStrings.size(), ll::impl::ImageAddressModeStrings>(std::forward<ll::ImageAddressMode>(value));
}


template<typename T>
inline ll::ImageAddressMode stringToImageAddressMode(T&& stringValue) {
    return impl::stringToEnum<ll::ImageAddressMode, T, ll::impl::ImageAddressModeStrings.size(), impl::ImageAddressModeStrings>(std::forward<T>(stringValue));
}


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
    ImageViewDescriptor& setIsSampled(bool isSampled);


    vk::SamplerCreateInfo getVkSamplerCreateInfo() const noexcept;

private:

    ll::ImageFilterMode filterMode;

    /**
     * Address mode for X, Y, Z axes
     */
    std::array<ll::ImageAddressMode, 3> addressMode;

    bool normalizedCoordinates {false};
    bool isSampled             {false};


friend class ImageView;
};


} // namespace ll

#endif /* LLUVIA_CORE_IMAGE_VIEW_DESCRIPTOR_H_ */

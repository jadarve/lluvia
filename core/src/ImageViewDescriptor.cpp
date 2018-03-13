#include "lluvia/core/ImageViewDescriptor.h"


namespace ll {

constexpr const char* STRING_IMAGE_FILTER_MODE_NEAREST = "NEAREST";
constexpr const char* STRING_IMAGE_FILTER_MODE_LINEAR  = "LINEAR";

constexpr const char* STRING_IMAGE_ADDRESS_MODE_REPEAT               = "REPEAT";
constexpr const char* STRING_IMAGE_ADDRESS_MODE_MIRRORED_REPEAT      = "MIRRORED_REPEAT";
constexpr const char* STRING_IMAGE_ADDRESS_MODE_CLAMP_TO_EDGE        = "CLAMP_TO_EDGE";
constexpr const char* STRING_IMAGE_ADDRESS_MODE_CLAMP_TO_BORDER      = "CLAMP_T_OBORDER";
constexpr const char* STRING_IMAGE_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE = "MIRROR_CLAMP_TO_EDGE";


std::string imageFilterModeToString(const ll::ImageFilterMode filterMode) {

    switch (filterMode) {
        case ll::ImageFilterMode::Nearest : return STRING_IMAGE_FILTER_MODE_NEAREST;
        case ll::ImageFilterMode::Linear  : return STRING_IMAGE_FILTER_MODE_LINEAR;
    }
}


ll::ImageFilterMode stringToImageFilterMode(const std::string& str) {

    if (str == STRING_IMAGE_FILTER_MODE_NEAREST) return ll::ImageFilterMode::Nearest;
    if (str == STRING_IMAGE_FILTER_MODE_LINEAR) return ll::ImageFilterMode::Linear;

    throw std::runtime_error("unknown image filter mode: " + str +
        ". Possible values are: " + STRING_IMAGE_FILTER_MODE_NEAREST + ", " + STRING_IMAGE_FILTER_MODE_LINEAR);
}


std::string imageAddressModeToString(const ll::ImageAddressMode addressMode) {

    switch (addressMode) {
        case ll::ImageAddressMode::Repeat            : return STRING_IMAGE_ADDRESS_MODE_REPEAT;
        case ll::ImageAddressMode::MirroredRepeat    : return STRING_IMAGE_ADDRESS_MODE_MIRRORED_REPEAT;
        case ll::ImageAddressMode::ClampToEdge       : return STRING_IMAGE_ADDRESS_MODE_CLAMP_TO_EDGE;
        case ll::ImageAddressMode::ClampToBorder     : return STRING_IMAGE_ADDRESS_MODE_CLAMP_TO_BORDER;
        case ll::ImageAddressMode::MirrorClampToEdge : return STRING_IMAGE_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
    }
}


ll::ImageAddressMode stringToImageAddressMode(const std::string& str) {

    if (str == STRING_IMAGE_ADDRESS_MODE_REPEAT)               return ll::ImageAddressMode::Repeat;
    if (str == STRING_IMAGE_ADDRESS_MODE_MIRRORED_REPEAT)      return ll::ImageAddressMode::MirroredRepeat;
    if (str == STRING_IMAGE_ADDRESS_MODE_CLAMP_TO_EDGE)        return ll::ImageAddressMode::ClampToEdge;
    if (str == STRING_IMAGE_ADDRESS_MODE_CLAMP_TO_BORDER)      return ll::ImageAddressMode::ClampToBorder;
    if (str == STRING_IMAGE_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE) return ll::ImageAddressMode::MirrorClampToEdge;

    throw std::runtime_error("unknown image address mode: " + str +
        ". Possible values are: " + STRING_IMAGE_ADDRESS_MODE_REPEAT + ", " + STRING_IMAGE_ADDRESS_MODE_MIRRORED_REPEAT
        + ", " + STRING_IMAGE_ADDRESS_MODE_CLAMP_TO_EDGE + ", " + STRING_IMAGE_ADDRESS_MODE_CLAMP_TO_BORDER
        + ", " + STRING_IMAGE_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE);   
}


ImageViewDescriptor& ImageViewDescriptor::setFilteringMode(ll::ImageFilterMode filterMode) {

    this->filterMode = filterMode;
    return *this;
}


ImageViewDescriptor& ImageViewDescriptor::setAddressMode(ll::ImageAddressMode addressMode) {

    for (auto& it : this->addressMode) {
        it = addressMode;
    }
    return *this;
}


ImageViewDescriptor& ImageViewDescriptor::setAddressMode(ll::ImageAxis axis, ll::ImageAddressMode addressMode) {

    this->addressMode[static_cast<uint32_t>(axis)] = addressMode;
    return *this;
}


ImageViewDescriptor& ImageViewDescriptor::setNormalizedCoordinates(bool normalizedCoordinates) {

    this->normalizedCoordinates = normalizedCoordinates;
    return *this;
}


vk::SamplerCreateInfo ImageViewDescriptor::getVkSamplerCreateInfo() const noexcept {

    auto getVkAddressMode = [](const ll::ImageAddressMode addressModeValue) {
        switch (addressModeValue) {
            case ll::ImageAddressMode::Repeat            : return vk::SamplerAddressMode::eRepeat;
            case ll::ImageAddressMode::MirroredRepeat    : return vk::SamplerAddressMode::eMirroredRepeat;
            case ll::ImageAddressMode::ClampToEdge       : return vk::SamplerAddressMode::eClampToEdge;
            case ll::ImageAddressMode::ClampToBorder     : return vk::SamplerAddressMode::eClampToBorder;
            case ll::ImageAddressMode::MirrorClampToEdge : return vk::SamplerAddressMode::eMirrorClampToEdge;
        }
    };

    auto info = vk::SamplerCreateInfo {}
                .setAddressModeU(getVkAddressMode(addressMode[static_cast<uint32_t>(ll::ImageAxis::U)]))
                .setAddressModeV(getVkAddressMode(addressMode[static_cast<uint32_t>(ll::ImageAxis::V)]))
                .setAddressModeW(getVkAddressMode(addressMode[static_cast<uint32_t>(ll::ImageAxis::W)]))
                .setUnnormalizedCoordinates(!normalizedCoordinates)
                .setAnisotropyEnable(false)
                .setMaxAnisotropy(1.0f)
                .setCompareEnable(false)
                .setCompareOp(vk::CompareOp::eAlways)
                .setMipmapMode(vk::SamplerMipmapMode::eNearest)
                .setMipLodBias(0.0f)
                .setMinLod(0.0f)
                .setMaxLod(0.0f);

    // filter mode
    switch (filterMode) {
        case ll::ImageFilterMode::Nearest:
            info.setMinFilter(vk::Filter::eNearest);
            info.setMagFilter(vk::Filter::eNearest);
            break;

        case ll::ImageFilterMode::Linear:
            info.setMinFilter(vk::Filter::eLinear);
            info.setMagFilter(vk::Filter::eLinear);
            break;
    }


    return info;
}

}; // namespace ll
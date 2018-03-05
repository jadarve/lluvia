#include "lluvia/core/ImageViewDescriptor.h"


namespace ll {

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
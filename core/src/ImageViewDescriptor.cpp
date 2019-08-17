/**
@file       ImageViewDescriptor.cpp
@brief      ImageViewDescriptor class and related enumerations and methods.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#include "lluvia/core/ImageViewDescriptor.h"


namespace ll {


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


ImageViewDescriptor& ImageViewDescriptor::setAddressMode(ll::ImageAddressMode tAddressMode) noexcept {

    for (auto& it : this->addressMode) {
        it = tAddressMode;
    }
    return *this;
}

ImageViewDescriptor& ImageViewDescriptor::setAddressMode(ll::ImageAxis axis, ll::ImageAddressMode tAddressMode) noexcept {
    
    this->addressMode[static_cast<uint32_t>(axis)] = tAddressMode;
    return *this;
}

}; // namespace ll
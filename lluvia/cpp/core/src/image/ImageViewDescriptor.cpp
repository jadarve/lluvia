/**
@file       ImageViewDescriptor.cpp
@brief      ImageViewDescriptor class and related enumerations and methods.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#include "lluvia/core/image/ImageViewDescriptor.h"


namespace ll {

ImageViewDescriptor::ImageViewDescriptor(const ll::ImageAddressMode addressMode,
                        const ll::ImageFilterMode filterMode,
                        const bool normalizedCoordinates,
                        const bool isSampled):
    m_filterMode {filterMode},
    m_addressMode {addressMode},
    m_normalizedCoordinates {normalizedCoordinates},
    m_isSampled {isSampled} {

}


ImageViewDescriptor& ImageViewDescriptor::setFilterMode(ll::ImageFilterMode filterMode) noexcept {

    this->m_filterMode = filterMode;
    return *this;
}


ll::ImageFilterMode ImageViewDescriptor::getFilterMode() const noexcept {

    return m_filterMode;
}


ImageViewDescriptor& ImageViewDescriptor::setAddressMode(ll::ImageAddressMode addressMode) noexcept {

    for (auto& it : this->m_addressMode) {
        it = addressMode;
    }
    return *this;
}


ImageViewDescriptor& ImageViewDescriptor::setAddressMode(ll::ImageAxis axis, ll::ImageAddressMode addressMode) noexcept {
    
    this->m_addressMode[static_cast<uint32_t>(axis)] = addressMode;
    return *this;
}


ll::ImageAddressMode ImageViewDescriptor::getAddressModeU() const noexcept {
    return m_addressMode[static_cast<uint32_t>(ll::ImageAxis::U)];
}


ll::ImageAddressMode ImageViewDescriptor::getAddressModeV() const noexcept {
    return m_addressMode[static_cast<uint32_t>(ll::ImageAxis::V)];
}


ll::ImageAddressMode ImageViewDescriptor::getAddressModeW() const noexcept {
    return m_addressMode[static_cast<uint32_t>(ll::ImageAxis::W)];
}


ImageViewDescriptor& ImageViewDescriptor::setNormalizedCoordinates(bool normalizedCoordinates) noexcept {

    this->m_normalizedCoordinates = normalizedCoordinates;
    return *this;
}


bool ImageViewDescriptor::isNormalizedCoordinates() const noexcept {
    return m_normalizedCoordinates;
}


ImageViewDescriptor& ImageViewDescriptor::setIsSampled(bool isSampled) noexcept {

    this->m_isSampled = isSampled;
    return *this;
}


bool ImageViewDescriptor::isSampled() const noexcept {

    return m_isSampled;
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
                .setAddressModeU(getVkAddressMode(m_addressMode[static_cast<uint32_t>(ll::ImageAxis::U)]))
                .setAddressModeV(getVkAddressMode(m_addressMode[static_cast<uint32_t>(ll::ImageAxis::V)]))
                .setAddressModeW(getVkAddressMode(m_addressMode[static_cast<uint32_t>(ll::ImageAxis::W)]))
                .setUnnormalizedCoordinates(!m_normalizedCoordinates)
                .setAnisotropyEnable(false)
                .setMaxAnisotropy(1.0f)
                .setCompareEnable(false)
                .setCompareOp(vk::CompareOp::eAlways)
                .setMipmapMode(vk::SamplerMipmapMode::eNearest)
                .setMipLodBias(0.0f)
                .setMinLod(0.0f)
                .setMaxLod(0.0f);

    // filter mode
    switch (m_filterMode) {
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
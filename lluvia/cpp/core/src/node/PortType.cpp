/**
@file       PortType.cpp
@brief      PortType class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#include "lluvia/core/node/PortType.h"

#include "lluvia/core/error.h"

#include <exception>

namespace ll {

vk::DescriptorType portTypeToVkDescriptorType(const ll::PortType& param)
{

    switch (param) {
    case ll::PortType::Buffer:
        return vk::DescriptorType::eStorageBuffer;

    case ll::PortType::ImageView:
        return vk::DescriptorType::eStorageImage;

    case ll::PortType::SampledImageView:
        return vk::DescriptorType::eCombinedImageSampler;

    case ll::PortType::UniformBuffer:
        return vk::DescriptorType::eUniformBuffer;
    }
}

ll::PortType vkDescriptorTypeToPortType(const vk::DescriptorType& vkDescType)
{

    switch (vkDescType) {
    case vk::DescriptorType::eStorageBuffer:
        return ll::PortType::Buffer;

    case vk::DescriptorType::eStorageImage:
        return ll::PortType::ImageView;

    case vk::DescriptorType::eCombinedImageSampler:
        return ll::PortType::SampledImageView;

    case vk::DescriptorType::eUniformBuffer:
        return ll::PortType::UniformBuffer;

    case vk::DescriptorType::eSampler:
    case vk::DescriptorType::eSampledImage:
    case vk::DescriptorType::eUniformTexelBuffer:
    case vk::DescriptorType::eStorageTexelBuffer:
    case vk::DescriptorType::eUniformBufferDynamic:
    case vk::DescriptorType::eStorageBufferDynamic:
    case vk::DescriptorType::eInputAttachment:
    default: // to cover descriptor types added by extensions
        throw std::system_error(createErrorCode(ll::ErrorCode::EnumConversionFailed), "cannot convert from Vulkan DescriptorType enum value to ll::PortType.");
    }
}

} // namespace ll

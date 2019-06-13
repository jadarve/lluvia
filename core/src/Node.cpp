/**
@file       Node.cpp
@brief      Node class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#include "lluvia/core/Node.h"

#include "lluvia/core/error.h"

#include <exception>

namespace ll {


vk::DescriptorType portTypeToVkDescriptorType(const ll::PortType& param) {

    switch (param) {
        case ll::PortType::Buffer:
            return vk::DescriptorType::eStorageBuffer;

        case ll::PortType::ImageView:
            return vk::DescriptorType::eStorageImage;

        case ll::PortType::SampledImageView:
            return vk::DescriptorType::eCombinedImageSampler;
    }
}


ll::PortType vkDescriptorTypeToPortType(const vk::DescriptorType& vkDescType) {

    switch (vkDescType) {
        case vk::DescriptorType::eStorageBuffer:
            return ll::PortType::Buffer;

        case vk::DescriptorType::eStorageImage:
            return ll::PortType::ImageView;

        case vk::DescriptorType::eCombinedImageSampler:
            return ll::PortType::SampledImageView;

        case vk::DescriptorType::eSampler:
        case vk::DescriptorType::eSampledImage:
        case vk::DescriptorType::eUniformTexelBuffer:
        case vk::DescriptorType::eStorageTexelBuffer:
        case vk::DescriptorType::eUniformBuffer:
        case vk::DescriptorType::eUniformBufferDynamic:
        case vk::DescriptorType::eStorageBufferDynamic:
        case vk::DescriptorType::eInputAttachment:
        default: // to cover descriptor types added by extensions
            throw std::system_error(createErrorCode(ll::ErrorCode::EnumConversionFailed), "cannot convert from Vulkan DescriptorType enum value to ll::PortType.");
    }
}


void Node::setState(const ll::NodeState tState) {

    // state transition
    switch(state) {
        case ll::NodeState::Created:
        
            switch(tState) {
                case ll::NodeState::Created:
                    // do nothing
                break;
                case ll::NodeState::Init:
                    state = tState;
                    onInit();
                break;
            }
        break;

        case ll::NodeState::Init:

            switch(state) {
                case ll::NodeState::Created:
                    // TODO: what to do?
                break;
                case ll::NodeState::Init:
                    // TODO: throw exception, node already in init state
                    throw std::runtime_error("Node alread in init state");
                break;
            }
        break;
    }

}

ll::NodeState Node::getState() const noexcept {
    return state;
}

} // namespace ll

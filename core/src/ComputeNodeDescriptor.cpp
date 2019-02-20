/**
@file       ComputeNodeDescriptor.cpp
@brief      ComputeNodeDescriptor class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#include "lluvia/core/ComputeNodeDescriptor.h"

#include "lluvia/core/error.h"
#include "lluvia/core/Program.h"

#include <cassert>
#include <exception>
#include <vector>
#include <iostream>

namespace ll {


vk::DescriptorType parameterTypeToVkDescriptorType(const ll::ParameterType& param) {

    switch (param) {
        case ll::ParameterType::Buffer:
            return vk::DescriptorType::eStorageBuffer;

        case ll::ParameterType::ImageView:
            return vk::DescriptorType::eStorageImage;

        case ll::ParameterType::SampledImageView:
            return vk::DescriptorType::eCombinedImageSampler;
    }
}


ll::ParameterType vkDescriptorTypeToParameterType(const vk::DescriptorType& vkDescType) {

    switch (vkDescType) {
        case vk::DescriptorType::eStorageBuffer:
            return ll::ParameterType::Buffer;

        case vk::DescriptorType::eStorageImage:
            return ll::ParameterType::ImageView;

        case vk::DescriptorType::eCombinedImageSampler:
            return ll::ParameterType::SampledImageView;

        case vk::DescriptorType::eSampler:
        case vk::DescriptorType::eSampledImage:
        case vk::DescriptorType::eUniformTexelBuffer:
        case vk::DescriptorType::eStorageTexelBuffer:
        case vk::DescriptorType::eUniformBuffer:
        case vk::DescriptorType::eUniformBufferDynamic:
        case vk::DescriptorType::eStorageBufferDynamic:
        case vk::DescriptorType::eInputAttachment:
            throw std::system_error(createErrorCode(ll::ErrorCode::EnumConversionFailed), "cannot convert from Vulkan DescriptorType enum value to ll::ParameterType.");
    }
}


ComputeNodeDescriptor& ComputeNodeDescriptor::setProgram(const std::shared_ptr<ll::Program>& program) {

    this->program = program;
    return *this;
}


ComputeNodeDescriptor& ComputeNodeDescriptor::setFunctionName(const std::string& name) {

    functionName = name;
    return *this;
}


ComputeNodeDescriptor& ComputeNodeDescriptor::addParameter(const ll::ParameterType param) {

    auto paramBinding = vk::DescriptorSetLayoutBinding {}
                            .setBinding(static_cast<uint32_t>(parameterBindings.size()))
                            .setDescriptorCount(1)
                            .setDescriptorType(parameterTypeToVkDescriptorType(param))
                            .setStageFlags(vk::ShaderStageFlagBits::eCompute)
                            .setPImmutableSamplers(nullptr);

    parameterBindings.push_back(paramBinding);
    return *this;
}


size_t ComputeNodeDescriptor::getParameterCount() const noexcept {
    return parameterBindings.size();
}


ll::ParameterType ComputeNodeDescriptor::getParameterTypeAt(const size_t& i) const {
    
    const auto& binding = parameterBindings.at(i);
    return ll::vkDescriptorTypeToParameterType(binding.descriptorType);
}


ComputeNodeDescriptor& ComputeNodeDescriptor::setGridX(const uint32_t x) noexcept {

    assert(x >= 1);
    globalGroup.x = x;
    return *this;
}


ComputeNodeDescriptor& ComputeNodeDescriptor::setGridY(const uint32_t y) noexcept {

    assert(y >= 1);
    globalGroup.y = y;
    return *this;
}


ComputeNodeDescriptor& ComputeNodeDescriptor::setGridZ(const uint32_t z) noexcept {

    assert(z >= 1);
    globalGroup.z = z;
    return *this;
}

ComputeNodeDescriptor& ComputeNodeDescriptor::setGridShape(const ll::vec3ui& shape) noexcept {

    assert(shape.x >= 1);
    assert(shape.y >= 1);
    assert(shape.z >= 1);

    globalGroup = shape;
    return *this;
}


ComputeNodeDescriptor& ComputeNodeDescriptor::setLocalX(const uint32_t x) noexcept {

    assert(x >= 1);
    localGroup.x = x;
    return *this;
}


ComputeNodeDescriptor& ComputeNodeDescriptor::setLocalY(const uint32_t y) noexcept {

    assert(y >= 1);
    localGroup.y = y;
    return *this;
}


ComputeNodeDescriptor& ComputeNodeDescriptor::setLocalZ(const uint32_t z) noexcept {

    assert(z >= 1);
    localGroup.z = z;
    return *this;
}


ComputeNodeDescriptor& ComputeNodeDescriptor::setLocalShape(const ll::vec3ui& shape) noexcept {

    assert(shape.x >= 1);
    assert(shape.y >= 1);
    assert(shape.z >= 1);

    localGroup = shape;
    return *this;
}

std::vector<vk::DescriptorPoolSize> ComputeNodeDescriptor::getDescriptorPoolSizes() const noexcept {

    auto pushDescriptorPoolSize = [this](const vk::DescriptorType type, std::vector<vk::DescriptorPoolSize>& v) {

        const auto count = countDescriptorType(type);
        if (count > 0) {
            v.push_back({type, count});
        }
    };

    std::vector<vk::DescriptorPoolSize> poolSizes;
    pushDescriptorPoolSize(vk::DescriptorType::eStorageBuffer, poolSizes);
    pushDescriptorPoolSize(vk::DescriptorType::eStorageImage, poolSizes);
    pushDescriptorPoolSize(vk::DescriptorType::eCombinedImageSampler, poolSizes);
        
    return poolSizes;
}


std::string ComputeNodeDescriptor::getFunctionName() const noexcept {
    return functionName;
}


ll::vec3ui ComputeNodeDescriptor::getGridShape() const noexcept {
    return globalGroup;
}


ll::vec3ui ComputeNodeDescriptor::getLocalShape() const noexcept {
    return localGroup;
}


uint32_t ComputeNodeDescriptor::getGridX() const noexcept {
    return globalGroup.x;
}


uint32_t ComputeNodeDescriptor::getGridY() const noexcept {
    return globalGroup.y;
}


uint32_t ComputeNodeDescriptor::getGridZ() const noexcept {
    return globalGroup.z;
}


uint32_t ComputeNodeDescriptor::getLocalX() const noexcept {
    return localGroup.x;
}


uint32_t ComputeNodeDescriptor::getLocalY() const noexcept {
    return localGroup.y;
}


uint32_t ComputeNodeDescriptor::getLocalZ() const noexcept {
    return localGroup.z;
}


std::vector<vk::DescriptorSetLayoutBinding> ComputeNodeDescriptor::getParameterBindings() const noexcept {
    return parameterBindings;
}


std::shared_ptr<ll::Program> ComputeNodeDescriptor::getProgram() const noexcept {
    return program;
}


uint32_t ComputeNodeDescriptor::getStorageBufferCount() const noexcept {
    return countDescriptorType(vk::DescriptorType::eStorageBuffer);
}


uint32_t ComputeNodeDescriptor::getStorageImageViewCount() const noexcept {
    return countDescriptorType(vk::DescriptorType::eStorageImage);
}


uint32_t ComputeNodeDescriptor::getSampledImageViewCount() const noexcept {
    return countDescriptorType(vk::DescriptorType::eCombinedImageSampler);
}


uint32_t ComputeNodeDescriptor::countDescriptorType(const vk::DescriptorType type) const noexcept {

    auto count = uint32_t {0};
    for (const auto& it : parameterBindings) {
        count += static_cast<uint32_t>(it.descriptorType == type);
    }
    return count;
}

} // namespace ll
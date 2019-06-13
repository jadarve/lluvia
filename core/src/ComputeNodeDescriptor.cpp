/**
@file       ComputeNodeDescriptor.cpp
@brief      ComputeNodeDescriptor class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#include "lluvia/core/ComputeNodeDescriptor.h"

#include "lluvia/core/error.h"
#include "lluvia/core/utils.h"
#include "lluvia/core/Program.h"

#include <cassert>
#include <exception>
#include <stdexcept>
#include <vector>
#include <iostream>

namespace ll {


ComputeNodeDescriptor& ComputeNodeDescriptor::setProgram(const std::shared_ptr<ll::Program>& tProgram) noexcept {

    this->m_program = tProgram;
    return *this;
}

ComputeNodeDescriptor& ComputeNodeDescriptor::setProgram(const std::shared_ptr<ll::Program>& tProgram, const std::string& tFunctionName) noexcept {
    
    setProgram(tProgram);
    setFunctionName(tFunctionName);
    return *this;
}


ComputeNodeDescriptor& ComputeNodeDescriptor::setFunctionName(const std::string& name) noexcept {

    m_functionName = name;
    return *this;
}


// ComputeNodeDescriptor& ComputeNodeDescriptor::addParameter(const ll::ParameterType param) {

//     auto paramBinding = vk::DescriptorSetLayoutBinding {}
//                             .setBinding(static_cast<uint32_t>(parameterBindings.size()))
//                             .setDescriptorCount(1)
//                             .setDescriptorType(parameterTypeToVkDescriptorType(param))
//                             .setStageFlags(vk::ShaderStageFlagBits::eCompute)
//                             .setPImmutableSamplers(nullptr);

//     parameterBindings.push_back(paramBinding);
//     return *this;
// }


ComputeNodeDescriptor& ComputeNodeDescriptor::addPort(const ll::PortDescriptor& port) {

    m_ports.push_back(port);
    return *this;
}


ComputeNodeDescriptor& ComputeNodeDescriptor::addPorts(const std::initializer_list<ll::PortDescriptor>& ports) {

    for(const auto port : ports) {
        addPort(port);
    }

    return *this;
}


size_t ComputeNodeDescriptor::getParameterCount() const noexcept {
    return m_parameterBindings.size();
}


// ll::ParameterType ComputeNodeDescriptor::getParameterTypeAt(const size_t& i) const {
    
//     const auto& binding = parameterBindings.at(i);
//     return ll::vkDescriptorTypeToParameterType(binding.descriptorType);
// }


ComputeNodeDescriptor& ComputeNodeDescriptor::setGridX(const uint32_t x) noexcept {
    m_gridShape.x = x;
    return *this;
}


ComputeNodeDescriptor& ComputeNodeDescriptor::setGridY(const uint32_t y) noexcept {
    m_gridShape.y = y;
    return *this;
}


ComputeNodeDescriptor& ComputeNodeDescriptor::setGridZ(const uint32_t z) noexcept {
    m_gridShape.z = z;
    return *this;
}

ComputeNodeDescriptor& ComputeNodeDescriptor::setGridShape(const ll::vec3ui& shape) noexcept {

    m_gridShape = shape;
    return *this;
}

ComputeNodeDescriptor& ComputeNodeDescriptor::configureGridShape(const ll::vec3ui& globalShape) noexcept {

    m_gridShape = ll::configureGridShape(m_localShape, globalShape);
    return *this;
}

ComputeNodeDescriptor& ComputeNodeDescriptor::setLocalX(const uint32_t x) noexcept {
    m_localShape.x = x;
    return *this;
}


ComputeNodeDescriptor& ComputeNodeDescriptor::setLocalY(const uint32_t y) noexcept {
    m_localShape.y = y;
    return *this;
}


ComputeNodeDescriptor& ComputeNodeDescriptor::setLocalZ(const uint32_t z) noexcept {
    m_localShape.z = z;
    return *this;
}


ComputeNodeDescriptor& ComputeNodeDescriptor::setLocalShape(const ll::vec3ui& shape) noexcept {
    m_localShape = shape;
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
    return m_functionName;
}


ll::vec3ui ComputeNodeDescriptor::getGridShape() const noexcept {
    return m_gridShape;
}


ll::vec3ui ComputeNodeDescriptor::getLocalShape() const noexcept {
    return m_localShape;
}


uint32_t ComputeNodeDescriptor::getGridX() const noexcept {
    return m_gridShape.x;
}


uint32_t ComputeNodeDescriptor::getGridY() const noexcept {
    return m_gridShape.y;
}


uint32_t ComputeNodeDescriptor::getGridZ() const noexcept {
    return m_gridShape.z;
}


uint32_t ComputeNodeDescriptor::getLocalX() const noexcept {
    return m_localShape.x;
}


uint32_t ComputeNodeDescriptor::getLocalY() const noexcept {
    return m_localShape.y;
}


uint32_t ComputeNodeDescriptor::getLocalZ() const noexcept {
    return m_localShape.z;
}


std::vector<vk::DescriptorSetLayoutBinding> ComputeNodeDescriptor::getParameterBindings() const noexcept {
    return m_parameterBindings;
}


std::shared_ptr<ll::Program> ComputeNodeDescriptor::getProgram() const noexcept {
    return m_program;
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
    for (const auto& it : m_parameterBindings) {
        count += static_cast<uint32_t>(it.descriptorType == type);
    }
    return count;
}



///////////////////////////////////////////////////////////////

// std::vector<vk::DescriptorPoolSize> ComputeNodeDescriptor::getDescriptorPoolSizes() const noexcept {

//     auto pushDescriptorPoolSize = [this](const vk::DescriptorType type, std::vector<vk::DescriptorPoolSize>& v) {

//         const auto count = countDescriptorType(type);
//         if (count > 0) {
//             v.push_back({type, count});
//         }
//     };

//     std::vector<vk::DescriptorPoolSize> poolSizes;
//     pushDescriptorPoolSize(vk::DescriptorType::eStorageBuffer, poolSizes);
//     pushDescriptorPoolSize(vk::DescriptorType::eStorageImage, poolSizes);
//     pushDescriptorPoolSize(vk::DescriptorType::eCombinedImageSampler, poolSizes);
        
//     return poolSizes;
// }


// ll::ParameterType ComputeNodeDescriptor::getParameterTypeAt(const size_t& i) const {
    
//     const auto& binding = parameterBindings.at(i);
//     return ll::vkDescriptorTypeToParameterType(binding.descriptorType);
// }


// ComputeNodeDescriptor& ComputeNodeDescriptor::addParameter(const ll::ParameterType param) {

//     auto paramBinding = vk::DescriptorSetLayoutBinding {}
//                             .setBinding(static_cast<uint32_t>(parameterBindings.size()))
//                             .setDescriptorCount(1)
//                             .setDescriptorType(parameterTypeToVkDescriptorType(param))
//                             .setStageFlags(vk::ShaderStageFlagBits::eCompute)
//                             .setPImmutableSamplers(nullptr);

//     parameterBindings.push_back(paramBinding);
//     return *this;
// }

} // namespace ll
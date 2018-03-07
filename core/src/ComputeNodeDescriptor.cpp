#include "lluvia/core/ComputeNodeDescriptor.h"

#include "lluvia/core/Program.h"

#include <cassert>
#include <vector>
#include <iostream>

namespace ll {

using namespace std;

ComputeNodeDescriptor& ComputeNodeDescriptor::setProgram(std::shared_ptr<ll::Program> program) {
    this->program = program;
    return *this;
}


ComputeNodeDescriptor& ComputeNodeDescriptor::setFunctionName(const std::string& name) {
    functionName = name;
    return *this;
}


ComputeNodeDescriptor& ComputeNodeDescriptor::addBufferParameter() {

    auto param = vk::DescriptorSetLayoutBinding {
        static_cast<uint32_t>(parameterBindings.size()),
        vk::DescriptorType::eStorageBuffer,
        1,
        vk::ShaderStageFlagBits::eCompute,
        nullptr
    };

    parameterBindings.push_back(param);
    return *this;
}


ComputeNodeDescriptor& ComputeNodeDescriptor::addCombinedImageSamplerParameter() {

    auto param = vk::DescriptorSetLayoutBinding {
        static_cast<uint32_t>(parameterBindings.size()),
        vk::DescriptorType::eCombinedImageSampler,
        1,
        vk::ShaderStageFlagBits::eCompute,
        nullptr
    };

    parameterBindings.push_back(param);
    return *this;
}


ComputeNodeDescriptor& ComputeNodeDescriptor::setGlobalX(const uint32_t x) noexcept {
    assert(x >= 1);
    globalGroup[0] = x;
    return *this;
}


ComputeNodeDescriptor& ComputeNodeDescriptor::setGlobalY(const uint32_t y) noexcept {
    assert(y >= 1);
    globalGroup[1] = y;
    return *this;
}


ComputeNodeDescriptor& ComputeNodeDescriptor::setGlobalZ(const uint32_t z) noexcept {
    assert(z >= 1);
    globalGroup[2] = z;
    return *this;
}


ComputeNodeDescriptor& ComputeNodeDescriptor::setLocalX(const uint32_t x) noexcept {
    assert(x >= 1);
    localGroup[0] = x;
    return *this;
}


ComputeNodeDescriptor& ComputeNodeDescriptor::setLocalY(const uint32_t y) noexcept {
    assert(y >= 1);
    localGroup[1] = y;
    return *this;
}


ComputeNodeDescriptor& ComputeNodeDescriptor::setLocalZ(const uint32_t z) noexcept {
    assert(z >= 1);
    localGroup[2] = z;
    return *this;
}


std::vector<vk::DescriptorPoolSize> ComputeNodeDescriptor::getDescriptorPoolSizes() const noexcept {

    vector<vk::DescriptorPoolSize> poolSizes{
        {vk::DescriptorType::eStorageBuffer, getStorageBufferCount()},
        {vk::DescriptorType::eCombinedImageSampler, getCombinedImageSamplerCount()}
    };
    
    return poolSizes;
}


std::string ComputeNodeDescriptor::getFunctionName() const noexcept {
    return functionName;
}


std::array<uint32_t, 3> ComputeNodeDescriptor::getGlobalGroup() const noexcept {
    return globalGroup;
}


std::array<uint32_t, 3> ComputeNodeDescriptor::getLocalGroup() const noexcept {
    return localGroup;
}


uint32_t ComputeNodeDescriptor::getGlobalX() const noexcept {
    return globalGroup[0];
}


uint32_t ComputeNodeDescriptor::getGlobalY() const noexcept {
    return globalGroup[1];
}


uint32_t ComputeNodeDescriptor::getGlobalZ() const noexcept {
    return globalGroup[2];
}


uint32_t ComputeNodeDescriptor::getLocalX() const noexcept {
    return localGroup[0];
}


uint32_t ComputeNodeDescriptor::getLocalY() const noexcept {
    return localGroup[1];
}


uint32_t ComputeNodeDescriptor::getLocalZ() const noexcept {
    return localGroup[2];
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


uint32_t ComputeNodeDescriptor::getCombinedImageSamplerCount() const noexcept {
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
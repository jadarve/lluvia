/**
@file       ComputeNode.cpp
@brief      ComputeNode class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#include "lluvia/core/ComputeNode.h"

#include "lluvia/core/Buffer.h"
#include "lluvia/core/ComputeNodeDescriptor.h"
#include "lluvia/core/error.h"
#include "lluvia/core/Image.h"
#include "lluvia/core/ImageView.h"
#include "lluvia/core/Object.h"
#include "lluvia/core/Program.h"

#include <algorithm>
#include <iostream>

namespace ll {

using namespace std;


ComputeNode::ComputeNode(const std::shared_ptr<const ll::Session>& session, const vk::Device& device, const ll::ComputeNodeDescriptor& descriptor):
    device       {device},
    descriptor   {descriptor},
    session      {session} {

    
    if (descriptor.program == nullptr) {
        throw std::system_error(createErrorCode(ll::ErrorCode::InvalidShaderProgram), "Shader program cannot be null.");
    }

    if (descriptor.functionName.empty()) {
        throw std::system_error(createErrorCode(ll::ErrorCode::InvalidShaderFunctionName), "Shader function name must be different than empty string.");
    }

    objects.resize(descriptor.parameterBindings.size());

    /////////////////////////////////////////////
    // Specialization constants
    /////////////////////////////////////////////

    const size_t size = sizeof(uint32_t);
    auto specializationMapEntries = vector<vk::SpecializationMapEntry> {
        {1, 0*size, size},
        {2, 1*size, size},
        {3, 2*size, size}
    };

    auto specializationInfo = vk::SpecializationInfo()
        .setMapEntryCount(static_cast<int>(specializationMapEntries.size()))
        .setPMapEntries(specializationMapEntries.data())
        .setDataSize(sizeof(ll::vec3ui))
        .setPData(&descriptor.localShape);

    /////////////////////////////////////////////
    // Pipeline stage info
    /////////////////////////////////////////////
    stageInfo = vk::PipelineShaderStageCreateInfo()
        .setStage(vk::ShaderStageFlagBits::eCompute)
        .setModule(descriptor.program->getShaderModule())
        .setPName(descriptor.functionName.c_str())
        .setPSpecializationInfo(&specializationInfo);


    /////////////////////////////////////////////
    // Descriptor pool and descriptor set
    /////////////////////////////////////////////
    auto descLayoutInfo = vk::DescriptorSetLayoutCreateInfo()
        .setBindingCount(static_cast<uint32_t>(descriptor.parameterBindings.size()))
        .setPBindings(descriptor.parameterBindings.data());

    descriptorSetLayout = device.createDescriptorSetLayout(descLayoutInfo);

    descriptorPoolSizes = descriptor.getDescriptorPoolSizes();
    descriptorPoolCreateInfo = vk::DescriptorPoolCreateInfo()
        .setMaxSets(1)
        .setPoolSizeCount(static_cast<uint32_t>(descriptorPoolSizes.size()))
        .setPPoolSizes(descriptorPoolSizes.data());

    device.createDescriptorPool(&descriptorPoolCreateInfo, nullptr, &descriptorPool);

    // only one descriptor set for this Node object
    vk::DescriptorSetAllocateInfo descSetAllocInfo = vk::DescriptorSetAllocateInfo()
        .setDescriptorPool(descriptorPool)
        .setDescriptorSetCount(1)
        .setPSetLayouts(&descriptorSetLayout);

    device.allocateDescriptorSets(&descSetAllocInfo, &descriptorSet);


    /////////////////////////////////////////////
    // Compute pipeline
    /////////////////////////////////////////////
    vk::PipelineLayoutCreateInfo pipeLayoutInfo = vk::PipelineLayoutCreateInfo()
        .setSetLayoutCount(1)
        .setPSetLayouts(&descriptorSetLayout);

    pipelineLayout = device.createPipelineLayout(pipeLayoutInfo);
    vk::ComputePipelineCreateInfo computePipeInfo = vk::ComputePipelineCreateInfo()
        .setStage(stageInfo)
        .setLayout(pipelineLayout);

    // create the compute pipeline
    pipeline = device.createComputePipeline(nullptr, computePipeInfo);
}


ComputeNode::~ComputeNode() {

    device.destroyPipeline(pipeline, nullptr);
    device.destroyPipelineLayout(pipelineLayout, nullptr);
    device.destroyDescriptorPool(descriptorPool, nullptr);
    device.destroyDescriptorSetLayout(descriptorSetLayout);
}


std::string ComputeNode::getFunctionName() const noexcept {
    return descriptor.functionName;
}


std::shared_ptr<ll::Program> ComputeNode::getProgram() const noexcept{
    return descriptor.program;
}


const ll::ComputeNodeDescriptor& ComputeNode::getDescriptor() const noexcept {
    return descriptor;
}


uint32_t ComputeNode::getLocalX() const noexcept {
    return descriptor.getLocalX();
}


uint32_t ComputeNode::getLocalY() const noexcept {
    return descriptor.getLocalY();
}


uint32_t ComputeNode::getLocalZ() const noexcept {
    return descriptor.getLocalZ();
}

ll::vec3ui ComputeNode::getLocalShape() const noexcept {
    return descriptor.getLocalShape();
}


uint32_t ComputeNode::getGridX() const noexcept {
    return descriptor.getGridX();
}


void ComputeNode::setGridX(const uint32_t x) noexcept {
    descriptor.setGridX(x);
}


uint32_t ComputeNode::getGridY() const noexcept {
    return descriptor.getGridY();
}


void ComputeNode::setGridY(const uint32_t y) noexcept {
    descriptor.setGridY(y);
}


uint32_t ComputeNode::getGridZ() const noexcept {
    return descriptor.getGridZ();
}


void ComputeNode::setGridZ(const uint32_t z) noexcept {
    descriptor.setGridZ(z);
}

void ComputeNode::setGridShape(const ll::vec3ui& shape) noexcept {
    descriptor.setGridShape(shape);
}

ll::vec3ui ComputeNode::getGridShape() const noexcept {
    return descriptor.getGridShape();
}


size_t ComputeNode::getParameterCount() const noexcept {
    return objects.size();
}


std::shared_ptr<ll::Object> ComputeNode::getParameter(size_t index) const noexcept {
    assert(index < objects.size());
    return objects[index];
}


void ComputeNode::bind(uint32_t index, const std::shared_ptr<ll::Object>& obj) {

    switch (obj->getType()) {
        case ll::ObjectType::Buffer:
            bindBuffer(index, std::static_pointer_cast<ll::Buffer>(obj));
            break;

        case ll::ObjectType::ImageView:
            bindImageView(index, std::static_pointer_cast<ll::ImageView>(obj));
            break;

        default:
            throw std::system_error(createErrorCode(ll::ErrorCode::ParameterBindingError),
                "Unsupported object type: " + ll::objectTypeToString(obj->getType()));
    }
}


void ComputeNode::record(const vk::CommandBuffer& commandBuffer) const {

    commandBuffer.bindPipeline(vk::PipelineBindPoint::eCompute, pipeline);
    
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute,
                                     pipelineLayout,
                                     0,
                                     1,
                                     &descriptorSet,
                                     0,
                                     nullptr);

    commandBuffer.dispatch(descriptor.gridShape.x,
                           descriptor.gridShape.y,
                           descriptor.gridShape.z);
}


void ComputeNode::accept(ll::Visitor* visitor) {
    assert(visitor != nullptr);
}


void ComputeNode::bindBuffer(uint32_t index, const std::shared_ptr<ll::Buffer>& buffer) {

    // validate that buffer can be bound at index position.
    const auto& vkBinding = descriptor.parameterBindings.at(index);
    const auto  paramType = ll::vkDescriptorTypeToParameterType(vkBinding.descriptorType);

    if (paramType != ll::ParameterType::Buffer) {
        throw std::system_error(createErrorCode(ll::ErrorCode::ParameterBindingError), "Parameter of type ll::Buffer cannot be bound at position ["
            + std::to_string(index) + "] as parameter type is not ll::ParameterType::Buffer");
    }


    // binding
    objects[index] = buffer;

    auto descBufferInfo = vk::DescriptorBufferInfo()
        .setOffset(0)
        .setRange(VK_WHOLE_SIZE)
        .setBuffer(buffer->vkBuffer);

    auto writeDescSet = vk::WriteDescriptorSet()
        .setDescriptorType(vk::DescriptorType::eStorageBuffer)
        .setDstSet(descriptorSet)
        .setDstBinding(index)
        .setDescriptorCount(1)
        .setPBufferInfo(&descBufferInfo);

    // update the informacion of the descriptor set
    device.updateDescriptorSets(1, &writeDescSet, 0, nullptr);
}


void ComputeNode::bindImageView(uint32_t index, const std::shared_ptr<ll::ImageView>& imgView) {

    const auto isSampled = imgView->getDescriptor().isSampled();

    // validate that imgView can be bound at index position.
    const auto& vkBinding = descriptor.parameterBindings.at(index);
    const auto  paramType = ll::vkDescriptorTypeToParameterType(vkBinding.descriptorType);

    if (isSampled) {
        if (paramType != ll::ParameterType::SampledImageView) {
            throw std::system_error(createErrorCode(ll::ErrorCode::ParameterBindingError), "Parameter of type ll::Imageview (sampled) cannot be bound at position ["
                + std::to_string(index) + "] as parameter type is not ll::ParameterType::SampledImageView");
        }
    }
    else {
        if (paramType != ll::ParameterType::ImageView) {
            throw std::system_error(createErrorCode(ll::ErrorCode::ParameterBindingError), "Parameter of type ll::Imageview cannot be bound at position ["
                + std::to_string(index) + "] as parameter type is not ll::ParameterType::ImageView");
        }
    }


    // binding
    objects[index] = imgView;

    auto descImgInfo = vk::DescriptorImageInfo {}
        .setSampler(imgView->vkSampler)
        .setImageView(imgView->vkImageView)
        .setImageLayout(imgView->image->vkLayout);

    auto writeDescSet = vk::WriteDescriptorSet()
        .setDstSet(descriptorSet)
        .setDstBinding(index)
        .setDescriptorCount(1)
        .setPImageInfo(&descImgInfo);

    writeDescSet.setDescriptorType(isSampled? vk::DescriptorType::eCombinedImageSampler : vk::DescriptorType::eStorageImage);

    // update the informacion of the descriptor set
    device.updateDescriptorSets(1, &writeDescSet, 0, nullptr);
}


} // namespace ll

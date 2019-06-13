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
#include <stdexcept>

namespace ll {

using namespace std;


ComputeNode::ComputeNode(
    const std::shared_ptr<const ll::Session>& tSession,
    const vk::Device& tDevice,
    const ll::ComputeNodeDescriptor& tDescriptor):

    m_device       {tDevice},
    m_descriptor   {tDescriptor},
    m_session      {tSession} {

    ll::throwSystemErrorIf(m_descriptor.program == nullptr, ll::ErrorCode::InvalidShaderProgram, "Shader program cannot be null.");
    ll::throwSystemErrorIf(m_descriptor.functionName.empty(), ll::ErrorCode::InvalidShaderFunctionName, "Shader function name must be different than empty string.");
    ll::throwSystemErrorIf(m_descriptor.localShape.x == 0, ll::ErrorCode::InvalidLocalShape, "descriptor local shape X must be greater than zero");
    ll::throwSystemErrorIf(m_descriptor.localShape.y == 0, ll::ErrorCode::InvalidLocalShape, "descriptor local shape Y must be greater than zero");
    ll::throwSystemErrorIf(m_descriptor.localShape.z == 0, ll::ErrorCode::InvalidLocalShape, "descriptor local shape Z must be greater than zero");

    // TODO: compute parameter bindings
    m_objects.resize(m_descriptor.parameterBindings.size());

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
        .setPData(&m_descriptor.localShape);

    /////////////////////////////////////////////
    // Pipeline stage info
    /////////////////////////////////////////////
    m_stageInfo = vk::PipelineShaderStageCreateInfo()
        .setStage(vk::ShaderStageFlagBits::eCompute)
        .setModule(m_descriptor.program->getShaderModule())
        .setPName(m_descriptor.functionName.c_str())
        .setPSpecializationInfo(&specializationInfo);


    /////////////////////////////////////////////
    // Descriptor pool and descriptor set
    /////////////////////////////////////////////
    auto descLayoutInfo = vk::DescriptorSetLayoutCreateInfo()
        .setBindingCount(static_cast<uint32_t>(descriptor.parameterBindings.size()))
        .setPBindings(descriptor.parameterBindings.data());

    m_descriptorSetLayout = m_device.createDescriptorSetLayout(descLayoutInfo);

    m_descriptorPoolSizes = m_descriptor.getDescriptorPoolSizes();
    m_descriptorPoolCreateInfo = vk::DescriptorPoolCreateInfo()
        .setMaxSets(1)
        .setPoolSizeCount(static_cast<uint32_t>(descriptorPoolSizes.size()))
        .setPPoolSizes(m_descriptorPoolSizes.data());

    m_device.createDescriptorPool(&m_descriptorPoolCreateInfo, nullptr, &m_descriptorPool);

    // only one descriptor set for this Node object
    vk::DescriptorSetAllocateInfo descSetAllocInfo = vk::DescriptorSetAllocateInfo()
        .setDescriptorPool(m_descriptorPool)
        .setDescriptorSetCount(1)
        .setPSetLayouts(&m_descriptorSetLayout);

    m_device.allocateDescriptorSets(&descSetAllocInfo, &m_descriptorSet);


    /////////////////////////////////////////////
    // Compute pipeline
    /////////////////////////////////////////////
    vk::PipelineLayoutCreateInfo pipeLayoutInfo = vk::PipelineLayoutCreateInfo()
        .setSetLayoutCount(1)
        .setPSetLayouts(&m_descriptorSetLayout);

    m_pipelineLayout = m_device.createPipelineLayout(pipeLayoutInfo);
    vk::ComputePipelineCreateInfo computePipeInfo = vk::ComputePipelineCreateInfo()
        .setStage(m_stageInfo)
        .setLayout(m_pipelineLayout);

    // create the compute pipeline
    m_pipeline = m_device.createComputePipeline(nullptr, computePipeInfo);
}

ComputeNode::~ComputeNode() {

    m_device.destroyPipeline(m_pipeline, nullptr);
    m_device.destroyPipelineLayout(m_pipelineLayout, nullptr);
    m_device.destroyDescriptorPool(m_descriptorPool, nullptr);
    m_device.destroyDescriptorSetLayout(m_descriptorSetLayout);
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
// 
void ComputeNode::initParameterBindings() {

    for(const auto& port : m_descriptor.ports) {
        auto binding = vk::DescriptorSetLayoutBinding {}
                        .setBinding(port.binding)
                        .setDescriptorCount(1)
                        .setDescriptorType(ll::portTypeToVkDescriptorType(port.type))
                        .setStageFlags(vk::ShaderStageFlagBits::eCompute)
                        .setPImmutableSamplers(nullptr);
        m_parameterBindings.push_back(binding);
    }

}

ll::NodeType ComputeNode::getType() const noexcept {
    return ll::NodeType::Compute;
}


std::string ComputeNode::getFunctionName() const noexcept {
    return m_descriptor.functionName;
}


std::shared_ptr<ll::Program> ComputeNode::getProgram() const noexcept{
    return m_descriptor.program;
}


const ll::ComputeNodeDescriptor& ComputeNode::getDescriptor() const noexcept {
    return m_descriptor;
}


uint32_t ComputeNode::getLocalX() const noexcept {
    return m_descriptor.localShape.x;
}


uint32_t ComputeNode::getLocalY() const noexcept {
    return m_descriptor.localShape.y;
}


uint32_t ComputeNode::getLocalZ() const noexcept {
    return m_descriptor.localShape.z;
}

ll::vec3ui ComputeNode::getLocalShape() const noexcept {
    return m_descriptor.localShape;
}


uint32_t ComputeNode::getGridX() const noexcept {
    return m_descriptor.gridShape.x;
}


void ComputeNode::setGridX(const uint32_t x) noexcept {
    m_descriptor.gridShape.x = x;
}


uint32_t ComputeNode::getGridY() const noexcept {
    return m_descriptor.gridShape.y;
}


void ComputeNode::setGridY(const uint32_t y) noexcept {
    m_descriptor.gridShape.y = y;
}


uint32_t ComputeNode::getGridZ() const noexcept {
    return m_descriptor.gridShape.z;
}


void ComputeNode::setGridZ(const uint32_t z) noexcept {
    m_descriptor.gridShape.z = z;
}


void ComputeNode::setGridShape(const ll::vec3ui& shape) noexcept {
    m_descriptor.gridShape = shape;
}


void ComputeNode::configureGridShape(const ll::vec3ui& globalShape) noexcept {
    m_descriptor.configureGridShape(globalShape);
}


ll::vec3ui ComputeNode::getGridShape() const noexcept {
    return m_descriptor.gridShape;
}


size_t ComputeNode::getParameterCount() const noexcept {
    return m_objects.size();
}


std::shared_ptr<ll::Object> ComputeNode::getParameter(size_t index) const noexcept {
    return m_objects[index];
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

    ll::throwSystemErrorIf(m_descriptor.gridShape.x == 0, ll::ErrorCode::InvalidLocalShape, "descriptor grid shape X must be greater than zero");
    ll::throwSystemErrorIf(m_descriptor.gridShape.y == 0, ll::ErrorCode::InvalidLocalShape, "descriptor grid shape Y must be greater than zero");
    ll::throwSystemErrorIf(m_descriptor.gridShape.z == 0, ll::ErrorCode::InvalidLocalShape, "descriptor grid shape Z must be greater than zero");

    commandBuffer.bindPipeline(vk::PipelineBindPoint::eCompute, m_pipeline);
    
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute,
                                     m_pipelineLayout,
                                     0,
                                     1,
                                     &m_descriptorSet,
                                     0,
                                     nullptr);

    commandBuffer.dispatch(m_descriptor.gridShape.x,
                           m_descriptor.gridShape.y,
                           m_descriptor.gridShape.z);
}


void ComputeNode::accept(__attribute__((unused)) ll::Visitor* visitor) {

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
    const auto& vkBinding = m_descriptor.parameterBindings.at(index);
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
    m_objects[index] = imgView;

    auto descImgInfo = vk::DescriptorImageInfo {}
        .setSampler(imgView->vkSampler)
        .setImageView(imgView->vkImageView)
        .setImageLayout(imgView->image->vkLayout);

    auto writeDescSet = vk::WriteDescriptorSet()
        .setDstSet(m_descriptorSet)
        .setDstBinding(index)
        .setDescriptorCount(1)
        .setPImageInfo(&descImgInfo);

    writeDescSet.setDescriptorType(isSampled? vk::DescriptorType::eCombinedImageSampler : vk::DescriptorType::eStorageImage);

    // update the informacion of the descriptor set
    m_device.updateDescriptorSets(1, &writeDescSet, 0, nullptr);
}


} // namespace ll

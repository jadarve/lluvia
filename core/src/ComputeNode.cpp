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

    ll::throwSystemErrorIf(m_descriptor.m_program == nullptr, ll::ErrorCode::InvalidShaderProgram, "Shader program cannot be null.");
    ll::throwSystemErrorIf(m_descriptor.m_functionName.empty(), ll::ErrorCode::InvalidShaderFunctionName, "Shader function name must be different than empty string.");
    ll::throwSystemErrorIf(m_descriptor.m_localShape.x == 0, ll::ErrorCode::InvalidLocalShape, "descriptor local shape X must be greater than zero");
    ll::throwSystemErrorIf(m_descriptor.m_localShape.y == 0, ll::ErrorCode::InvalidLocalShape, "descriptor local shape Y must be greater than zero");
    ll::throwSystemErrorIf(m_descriptor.m_localShape.z == 0, ll::ErrorCode::InvalidLocalShape, "descriptor local shape Z must be greater than zero");

    initParameterBindings();

    
    m_objects.resize(m_parameterBindings.size());

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
        .setPData(&m_descriptor.m_localShape);

    /////////////////////////////////////////////
    // Pipeline stage info
    /////////////////////////////////////////////
    m_stageInfo = vk::PipelineShaderStageCreateInfo()
        .setStage(vk::ShaderStageFlagBits::eCompute)
        .setModule(m_descriptor.m_program->getShaderModule())
        .setPName(m_descriptor.m_functionName.c_str())
        .setPSpecializationInfo(&specializationInfo);


    /////////////////////////////////////////////
    // Descriptor pool and descriptor set
    /////////////////////////////////////////////
    auto descLayoutInfo = vk::DescriptorSetLayoutCreateInfo()
        .setBindingCount(static_cast<uint32_t>(m_parameterBindings.size()))
        .setPBindings(m_parameterBindings.data());

    m_descriptorSetLayout = m_device.createDescriptorSetLayout(descLayoutInfo);

    m_descriptorPoolSizes = getDescriptorPoolSizes();
    m_descriptorPoolCreateInfo = vk::DescriptorPoolCreateInfo()
        .setMaxSets(1)
        .setPoolSizeCount(static_cast<uint32_t>(m_descriptorPoolSizes.size()))
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

void ComputeNode::initParameterBindings() {

    for(const auto& port : m_descriptor.m_ports) {
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
    return m_descriptor.m_functionName;
}


std::shared_ptr<ll::Program> ComputeNode::getProgram() const noexcept{
    return m_descriptor.m_program;
}


const ll::ComputeNodeDescriptor& ComputeNode::getDescriptor() const noexcept {
    return m_descriptor;
}


uint32_t ComputeNode::getLocalX() const noexcept {
    return m_descriptor.m_localShape.x;
}


uint32_t ComputeNode::getLocalY() const noexcept {
    return m_descriptor.m_localShape.y;
}


uint32_t ComputeNode::getLocalZ() const noexcept {
    return m_descriptor.m_localShape.z;
}

ll::vec3ui ComputeNode::getLocalShape() const noexcept {
    return m_descriptor.m_localShape;
}


uint32_t ComputeNode::getGridX() const noexcept {
    return m_descriptor.m_gridShape.x;
}


void ComputeNode::setGridX(const uint32_t x) noexcept {
    m_descriptor.m_gridShape.x = x;
}


uint32_t ComputeNode::getGridY() const noexcept {
    return m_descriptor.m_gridShape.y;
}


void ComputeNode::setGridY(const uint32_t y) noexcept {
    m_descriptor.m_gridShape.y = y;
}


uint32_t ComputeNode::getGridZ() const noexcept {
    return m_descriptor.m_gridShape.z;
}


void ComputeNode::setGridZ(const uint32_t z) noexcept {
    m_descriptor.m_gridShape.z = z;
}


void ComputeNode::setGridShape(const ll::vec3ui& shape) noexcept {
    m_descriptor.m_gridShape = shape;
}


void ComputeNode::configureGridShape(const ll::vec3ui& globalShape) noexcept {
    m_descriptor.configureGridShape(globalShape);
}


ll::vec3ui ComputeNode::getGridShape() const noexcept {
    return m_descriptor.m_gridShape;
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

    ll::throwSystemErrorIf(m_descriptor.m_gridShape.x == 0, ll::ErrorCode::InvalidLocalShape, "descriptor grid shape X must be greater than zero");
    ll::throwSystemErrorIf(m_descriptor.m_gridShape.y == 0, ll::ErrorCode::InvalidLocalShape, "descriptor grid shape Y must be greater than zero");
    ll::throwSystemErrorIf(m_descriptor.m_gridShape.z == 0, ll::ErrorCode::InvalidLocalShape, "descriptor grid shape Z must be greater than zero");

    commandBuffer.bindPipeline(vk::PipelineBindPoint::eCompute, m_pipeline);
    
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute,
                                     m_pipelineLayout,
                                     0,
                                     1,
                                     &m_descriptorSet,
                                     0,
                                     nullptr);

    commandBuffer.dispatch(m_descriptor.m_gridShape.x,
                           m_descriptor.m_gridShape.y,
                           m_descriptor.m_gridShape.z);
}


void ComputeNode::accept(__attribute__((unused)) ll::Visitor* visitor) {

}


void ComputeNode::bindBuffer(uint32_t index, const std::shared_ptr<ll::Buffer>& buffer) {

    // validate that buffer can be bound at index position.
    const auto& vkBinding = m_parameterBindings.at(index);
    const auto  paramType = ll::vkDescriptorTypeToPortType(vkBinding.descriptorType);

    if (paramType != ll::PortType::Buffer) {
        throw std::system_error(createErrorCode(ll::ErrorCode::ParameterBindingError), "Parameter of type ll::Buffer cannot be bound at position ["
            + std::to_string(index) + "] as parameter type is not ll::ParameterType::Buffer");
    }


    // binding
    m_objects[index] = buffer;

    auto descBufferInfo = vk::DescriptorBufferInfo()
        .setOffset(0)
        .setRange(VK_WHOLE_SIZE)
        .setBuffer(buffer->vkBuffer);

    auto writeDescSet = vk::WriteDescriptorSet()
        .setDescriptorType(vk::DescriptorType::eStorageBuffer)
        .setDstSet(m_descriptorSet)
        .setDstBinding(index)
        .setDescriptorCount(1)
        .setPBufferInfo(&descBufferInfo);

    // update the informacion of the descriptor set
    m_device.updateDescriptorSets(1, &writeDescSet, 0, nullptr);
}


void ComputeNode::bindImageView(uint32_t index, const std::shared_ptr<ll::ImageView>& imgView) {

    const auto isSampled = imgView->getDescriptor().isSampled();

    // validate that imgView can be bound at index position.
    const auto& vkBinding = m_parameterBindings.at(index);
    const auto  paramType = ll::vkDescriptorTypeToPortType(vkBinding.descriptorType);

    if (isSampled) {
        if (paramType != ll::PortType::SampledImageView) {
            throw std::system_error(createErrorCode(ll::ErrorCode::ParameterBindingError), "Parameter of type ll::Imageview (sampled) cannot be bound at position ["
                + std::to_string(index) + "] as parameter type is not ll::PortType::SampledImageView");
        }
    }
    else {
        if (paramType != ll::PortType::ImageView) {
            throw std::system_error(createErrorCode(ll::ErrorCode::ParameterBindingError), "Parameter of type ll::Imageview cannot be bound at position ["
                + std::to_string(index) + "] as parameter type is not ll::PortType::ImageView");
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


std::vector<vk::DescriptorPoolSize> ComputeNode::getDescriptorPoolSizes() const noexcept {

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


uint32_t ComputeNode::countDescriptorType(const vk::DescriptorType type) const noexcept {

    auto count = uint32_t {0};
    for (const auto& it : m_parameterBindings) {
        count += static_cast<uint32_t>(it.descriptorType == type);
    }
    return count;
}


} // namespace ll

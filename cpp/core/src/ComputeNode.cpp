/**
@file       ComputeNode.cpp
@brief      ComputeNode class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#include "lluvia/core/ComputeNode.h"

#include "lluvia/core/Buffer.h"
#include "lluvia/core/CommandBuffer.h"
#include "lluvia/core/ComputeNodeDescriptor.h"
#include "lluvia/core/error.h"
#include "lluvia/core/Image.h"
#include "lluvia/core/ImageView.h"
#include "lluvia/core/Interpreter.h"
#include "lluvia/core/Object.h"
#include "lluvia/core/Program.h"
#include "lluvia/core/Session.h"

#include <algorithm>
#include <stdexcept>

namespace ll {

using namespace std;


ComputeNode::ComputeNode(
    const std::shared_ptr<ll::Session>& tSession,
    const vk::Device& tDevice,
    const ll::ComputeNodeDescriptor& tDescriptor):

    m_device       {tDevice},
    m_descriptor   {tDescriptor},
    m_session      {tSession} {

    ll::throwSystemErrorIf(m_descriptor.getProgram() == nullptr, ll::ErrorCode::InvalidShaderProgram, "Shader program cannot be null.");
    ll::throwSystemErrorIf(m_descriptor.getFunctionName().empty(), ll::ErrorCode::InvalidShaderFunctionName, "Shader function name must be different than empty string.");
    ll::throwSystemErrorIf(m_descriptor.getLocalX() == 0, ll::ErrorCode::InvalidLocalShape, "descriptor local shape X must be greater than zero");
    ll::throwSystemErrorIf(m_descriptor.getLocalY() == 0, ll::ErrorCode::InvalidLocalShape, "descriptor local shape Y must be greater than zero");
    ll::throwSystemErrorIf(m_descriptor.getLocalZ() == 0, ll::ErrorCode::InvalidLocalShape, "descriptor local shape Z must be greater than zero");

    initPortBindings();
}


ComputeNode::~ComputeNode() {

    m_device.destroyPipeline(m_pipeline, nullptr);
    m_device.destroyPipelineLayout(m_pipelineLayout, nullptr);
    m_device.destroyDescriptorPool(m_descriptorPool, nullptr);
    m_device.destroyDescriptorSetLayout(m_descriptorSetLayout);
}


void ComputeNode::initPortBindings() {

    m_parameterBindings = m_descriptor.getParameterBindings();

    /////////////////////////////////////////////
    // Descriptor pool and descriptor set
    /////////////////////////////////////////////
    auto descLayoutInfo = vk::DescriptorSetLayoutCreateInfo()
        .setBindingCount(static_cast<uint32_t>(m_parameterBindings.size()))
        .setPBindings(m_parameterBindings.data());

    m_descriptorSetLayout = m_device.createDescriptorSetLayout(descLayoutInfo);

    auto descriptorPoolSizes = getDescriptorPoolSizes();
    auto descriptorPoolCreateInfo = vk::DescriptorPoolCreateInfo()
        .setMaxSets(1)
        .setPoolSizeCount(static_cast<uint32_t>(descriptorPoolSizes.size()))
        .setPPoolSizes(descriptorPoolSizes.data());

    m_device.createDescriptorPool(&descriptorPoolCreateInfo, nullptr, &m_descriptorPool);

    // only one descriptor set for this Node object
    vk::DescriptorSetAllocateInfo descSetAllocInfo = vk::DescriptorSetAllocateInfo()
        .setDescriptorPool(m_descriptorPool)
        .setDescriptorSetCount(1)
        .setPSetLayouts(&m_descriptorSetLayout);

    m_device.allocateDescriptorSets(&descSetAllocInfo, &m_descriptorSet);
}


void ComputeNode::initPipeline() {

    /////////////////////////////////////////////
    // Specialization constants
    /////////////////////////////////////////////
    const size_t size = sizeof(uint32_t);
    auto specializationMapEntries = vector<vk::SpecializationMapEntry> {
        {1, 0*size, size},
        {2, 1*size, size},
        {3, 2*size, size}
    };

    auto localShape = m_descriptor.getLocalShape();
    auto specializationInfo = vk::SpecializationInfo()
        .setMapEntryCount(static_cast<int>(specializationMapEntries.size()))
        .setPMapEntries(specializationMapEntries.data())
        .setDataSize(sizeof(ll::vec3ui))
        .setPData(&localShape);

    auto stageInfo = vk::PipelineShaderStageCreateInfo()
        .setStage(vk::ShaderStageFlagBits::eCompute)
        .setModule(m_descriptor.getProgram()->getShaderModule())
        .setPName(m_descriptor.getFunctionName().c_str())
        .setPSpecializationInfo(&specializationInfo);

    /////////////////////////////////////////////
    // Compute pipeline
    /////////////////////////////////////////////
    vk::PipelineLayoutCreateInfo pipeLayoutInfo = vk::PipelineLayoutCreateInfo()
        .setSetLayoutCount(1)
        .setPSetLayouts(&m_descriptorSetLayout);

    m_pipelineLayout = m_device.createPipelineLayout(pipeLayoutInfo);
    vk::ComputePipelineCreateInfo computePipeInfo = vk::ComputePipelineCreateInfo()
        .setStage(stageInfo)
        .setLayout(m_pipelineLayout);

    // create the compute pipeline
    m_pipeline = m_device.createComputePipeline(nullptr, computePipeInfo);
}


ll::NodeType ComputeNode::getType() const noexcept {
    return ll::NodeType::Compute;
}


const std::shared_ptr<ll::Session>& ComputeNode::getSession() const noexcept {
    return m_session;
}


std::string ComputeNode::getFunctionName() const noexcept {
    return m_descriptor.getFunctionName();
}


std::shared_ptr<ll::Program> ComputeNode::getProgram() const noexcept{
    return m_descriptor.getProgram();
}


const ll::ComputeNodeDescriptor& ComputeNode::getDescriptor() const noexcept {
    return m_descriptor;
}


uint32_t ComputeNode::getLocalX() const noexcept {
    return m_descriptor.getLocalX();
}


uint32_t ComputeNode::getLocalY() const noexcept {
    return m_descriptor.getLocalY();
}


uint32_t ComputeNode::getLocalZ() const noexcept {
    return m_descriptor.getLocalZ();
}

ll::vec3ui ComputeNode::getLocalShape() const noexcept {
    return m_descriptor.getLocalShape();
}


uint32_t ComputeNode::getGridX() const noexcept {
    return m_descriptor.getGridX();
}


void ComputeNode::setGridX(const uint32_t x) noexcept {
    m_descriptor.setGridX(x);
}


uint32_t ComputeNode::getGridY() const noexcept {
    return m_descriptor.getGridY();
}


void ComputeNode::setGridY(const uint32_t y) noexcept {
    m_descriptor.setGridY(y);
}


uint32_t ComputeNode::getGridZ() const noexcept {
    return m_descriptor.getGridZ();
}


void ComputeNode::setGridZ(const uint32_t z) noexcept {
    m_descriptor.setGridZ(z);
}


void ComputeNode::setGridShape(const ll::vec3ui& shape) noexcept {
    m_descriptor.setGridShape(shape);
}


void ComputeNode::configureGridShape(const ll::vec3ui& globalShape) noexcept {
    m_descriptor.configureGridShape(globalShape);
}


ll::vec3ui ComputeNode::getGridShape() const noexcept {
    return m_descriptor.getGridShape();
}


std::shared_ptr<ll::Object> ComputeNode::getPort(const std::string& name) const {

    const auto it = m_objects.find(name);
    ll::throwSystemErrorIf(it == m_objects.cend(), ll::ErrorCode::KeyNotFound, "Port [" + name + "] not found.");

    return it->second;
}


void ComputeNode::bind(const std::string& name, const std::shared_ptr<ll::Object>& obj) {

    const auto& port = m_descriptor.getPort(name);

    // bind obj according to its type
    switch (obj->getType()) {
        case ll::ObjectType::Buffer:
            bindBuffer(port, std::static_pointer_cast<ll::Buffer>(obj));
            break;

        case ll::ObjectType::ImageView:
            bindImageView(port, std::static_pointer_cast<ll::ImageView>(obj));
            break;

        default:
            throw std::system_error(createErrorCode(ll::ErrorCode::PortBindingError),
                "Unsupported object type: " + ll::objectTypeToString(obj->getType()));
    }
}


void ComputeNode::record(ll::CommandBuffer& commandBuffer) const {

    ll::throwSystemErrorIf(getState() != ll::NodeState::Init, ll::ErrorCode::InvalidNodeState, "node must be in Init state before calling record()");

    auto vkCommandBuffer = commandBuffer.getVkCommandBuffer();

    ll::throwSystemErrorIf(m_descriptor.getGridX() == 0, ll::ErrorCode::InvalidLocalShape, "descriptor grid shape X must be greater than zero");
    ll::throwSystemErrorIf(m_descriptor.getGridY() == 0, ll::ErrorCode::InvalidLocalShape, "descriptor grid shape Y must be greater than zero");
    ll::throwSystemErrorIf(m_descriptor.getGridZ() == 0, ll::ErrorCode::InvalidLocalShape, "descriptor grid shape Z must be greater than zero");

    vkCommandBuffer.bindPipeline(vk::PipelineBindPoint::eCompute, m_pipeline);
    
    vkCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute,
                                     m_pipelineLayout,
                                     0,
                                     1,
                                     &m_descriptorSet,
                                     0,
                                     nullptr);

    // vkCommandBuffer.pushConstants(m_pipelineLayout, vk::ShaderStageFlagBits::eCompute, 0, nullptr);

    vkCommandBuffer.dispatch(m_descriptor.getGridX(),
                             m_descriptor.getGridY(),
                             m_descriptor.getGridZ());
}


void ComputeNode::onInit() {

    const auto builderName = m_descriptor.getBuilderName();
    if (!builderName.empty()) {

        constexpr const auto lua = R"(
            local builderName, node = ...
            local builder = ll.getNodeBuilder(builderName)
            builder.onNodeInit(node)
        )";

        m_session->getInterpreter()->loadAndRun<void>(lua, builderName, shared_from_this());

        // auto load = m_session->getInterpreter()->load(lua);
        // load(builderName, shared_from_this());
    }

    initPipeline();
}

void ComputeNode::bindBuffer(const ll::PortDescriptor& port, const std::shared_ptr<ll::Buffer>& buffer) {

    // validate that buffer can be bound at index position
    // in the vulkan descriptor set
    const auto& vkBinding = m_parameterBindings.at(port.binding);
    const auto  paramType = ll::vkDescriptorTypeToPortType(vkBinding.descriptorType);

    ll::throwSystemErrorIf(paramType != ll::PortType::Buffer,
        ll::ErrorCode::PortBindingError,
        "Parameter of type ll::Buffer cannot be bound at position ["
        + std::to_string(port.binding) + "] as parameter type is not ll::ParameterType::Buffer");

    // holds a reference to the object
    m_objects[port.name] = buffer;

    // update the informacion of the descriptor set
    auto descBufferInfo = vk::DescriptorBufferInfo()
        .setOffset(0)
        .setRange(VK_WHOLE_SIZE)
        .setBuffer(buffer->m_vkBuffer);

    auto writeDescSet = vk::WriteDescriptorSet()
        .setDescriptorType(vk::DescriptorType::eStorageBuffer)
        .setDstSet(m_descriptorSet)
        .setDstBinding(port.binding)
        .setDescriptorCount(1)
        .setPBufferInfo(&descBufferInfo);

    m_device.updateDescriptorSets(1, &writeDescSet, 0, nullptr);
}


void ComputeNode::bindImageView(const ll::PortDescriptor& port, const std::shared_ptr<ll::ImageView>& imgView) {

    const auto isSampled = imgView->getDescriptor().isSampled();

    // validate that imgView can be bound at index position.
    const auto& vkBinding = m_parameterBindings.at(port.binding);
    const auto  paramType = ll::vkDescriptorTypeToPortType(vkBinding.descriptorType);

    if (isSampled) {
        ll::throwSystemErrorIf(paramType != ll::PortType::SampledImageView, ll::ErrorCode::PortBindingError,
            "Parameter of type ll::Imageview (sampled) cannot be bound at position ["
            + std::to_string(port.binding) + "] as parameter type is not ll::PortType::SampledImageView");
    }
    else {
        ll::throwSystemErrorIf(paramType != ll::PortType::ImageView, ll::ErrorCode::PortBindingError,
            "Parameter of type ll::Imageview cannot be bound at position ["
            + std::to_string(port.binding) + "] as parameter type is not ll::PortType::ImageView");
    }

    // binding
    m_objects[port.name] = imgView;

    auto descImgInfo = vk::DescriptorImageInfo {}
        .setSampler(imgView->m_vkSampler)
        .setImageView(imgView->m_vkImageView)
        .setImageLayout(imgView->m_image->m_vkLayout);

    auto writeDescSet = vk::WriteDescriptorSet()
        .setDstSet(m_descriptorSet)
        .setDstBinding(port.binding)
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

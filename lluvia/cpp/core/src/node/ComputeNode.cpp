/**
@file       ComputeNode.cpp
@brief      ComputeNode class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#include "lluvia/core/node/ComputeNode.h"

#include "lluvia/core/buffer/Buffer.h"
#include "lluvia/core/CommandBuffer.h"
#include "lluvia/core/error.h"
#include "lluvia/core/image/Image.h"
#include "lluvia/core/image/ImageView.h"
#include "lluvia/core/Interpreter.h"
#include "lluvia/core/node/ComputeNodeDescriptor.h"
#include "lluvia/core/node/PushConstants.h"
#include "lluvia/core/Object.h"
#include "lluvia/core/Program.h"

#include "lluvia/core/vulkan/Device.h"

#include <algorithm>
#include <stdexcept>

namespace ll {

using namespace std;


ComputeNode::ComputeNode(const std::shared_ptr<ll::vulkan::Device>& device,
                         const ll::ComputeNodeDescriptor& descriptor,
                         const std::weak_ptr<ll::Interpreter>& interpreter) :

    m_device       {device},
    m_descriptor   {descriptor},
    m_interpreter  {interpreter} {

    ll::throwSystemErrorIf(m_descriptor.getProgram() == nullptr, ll::ErrorCode::InvalidShaderProgram, "Shader program cannot be null.");
    ll::throwSystemErrorIf(m_descriptor.getFunctionName().empty(), ll::ErrorCode::InvalidShaderFunctionName, "Shader function name must be different than empty string.");
    ll::throwSystemErrorIf(m_descriptor.getLocalX() == 0, ll::ErrorCode::InvalidLocalShape, "descriptor local shape X must be greater than zero");
    ll::throwSystemErrorIf(m_descriptor.getLocalY() == 0, ll::ErrorCode::InvalidLocalShape, "descriptor local shape Y must be greater than zero");
    ll::throwSystemErrorIf(m_descriptor.getLocalZ() == 0, ll::ErrorCode::InvalidLocalShape, "descriptor local shape Z must be greater than zero");

    initPortBindings();
}


ComputeNode::~ComputeNode() {

    m_device->get().destroyPipeline(m_pipeline, nullptr);
    m_device->get().destroyPipelineLayout(m_pipelineLayout, nullptr);
    m_device->get().destroyDescriptorPool(m_descriptorPool, nullptr);
    m_device->get().destroyDescriptorSetLayout(m_descriptorSetLayout);
}


void ComputeNode::initPortBindings() {

    m_parameterBindings = m_descriptor.getParameterBindings();

    /////////////////////////////////////////////
    // Descriptor pool and descriptor set
    /////////////////////////////////////////////
    auto descLayoutInfo = vk::DescriptorSetLayoutCreateInfo()
        .setBindingCount(static_cast<uint32_t>(m_parameterBindings.size()))
        .setPBindings(m_parameterBindings.data());

    m_descriptorSetLayout = m_device->get().createDescriptorSetLayout(descLayoutInfo);

    auto descriptorPoolSizes = getDescriptorPoolSizes();
    auto descriptorPoolCreateInfo = vk::DescriptorPoolCreateInfo()
        .setMaxSets(1)
        .setPoolSizeCount(static_cast<uint32_t>(descriptorPoolSizes.size()))
        .setPPoolSizes(descriptorPoolSizes.data());


    if (m_device->get().createDescriptorPool(&descriptorPoolCreateInfo, nullptr, &m_descriptorPool) != vk::Result::eSuccess) {

        // free previously allocated resources
        m_device->get().destroyDescriptorSetLayout(m_descriptorSetLayout);

        // then throw system error
        ll::throwSystemError(ll::ErrorCode::VulkanError, "error creating descriptor pool for compute node.");
    }

    // only one descriptor set for this Node object
    vk::DescriptorSetAllocateInfo descSetAllocInfo = vk::DescriptorSetAllocateInfo()
        .setDescriptorPool(m_descriptorPool)
        .setDescriptorSetCount(1)
        .setPSetLayouts(&m_descriptorSetLayout);

    if (m_device->get().allocateDescriptorSets(&descSetAllocInfo, &m_descriptorSet) != vk::Result::eSuccess) {

        // free previously allocated resources
        m_device->get().destroyDescriptorPool(m_descriptorPool, nullptr);
        m_device->get().destroyDescriptorSetLayout(m_descriptorSetLayout);

        ll::throwSystemError(ll::ErrorCode::VulkanError, "error allocating descriptor set.");
    }
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
    
    const auto& pushConstants = m_descriptor.getPushConstants();
    if (pushConstants.getSize() != 0) {
        auto pushConstantRange = vk::PushConstantRange()
            .setOffset(0)
            .setSize(static_cast<uint32_t>(pushConstants.getSize()))
            .setStageFlags(vk::ShaderStageFlagBits::eCompute);

        pipeLayoutInfo.setPushConstantRangeCount(1);
        pipeLayoutInfo.setPPushConstantRanges(&pushConstantRange);
    }

    m_pipelineLayout = m_device->get().createPipelineLayout(pipeLayoutInfo);
    vk::ComputePipelineCreateInfo computePipeInfo = vk::ComputePipelineCreateInfo()
        .setStage(stageInfo)
        .setLayout(m_pipelineLayout);

    // create the compute pipeline
    auto result = m_device->get().createComputePipeline(nullptr, computePipeInfo);
    ll::throwSystemErrorIf(result.result != vk::Result::eSuccess, ll::ErrorCode ::PipelineCreationError, "error creating vulkan compute pipeline for node.");

    m_pipeline = result.value;
}


ll::NodeType ComputeNode::getType() const noexcept {
    return ll::NodeType::Compute;
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


void ComputeNode::setPushConstants(const ll::PushConstants& constants) noexcept {
    m_descriptor.setPushConstants(constants);
}


const ll::PushConstants& ComputeNode::getPushConstants() const noexcept {
    return m_descriptor.getPushConstants();
}

void ComputeNode::setParameter(const std::string& name, const ll::Parameter& value) {
    m_descriptor.setParameter(name, value);
}

const ll::Parameter& ComputeNode::getParameter(const std::string& name) const {
    return m_descriptor.getParameter(name);
}

void ComputeNode::bind(const std::string& name, const std::shared_ptr<ll::Object>& obj) {

    const auto& port = m_descriptor.getPort(name);

    // validate if the object passed is valid under the port descriptor contract
    const auto validationResult = port.isValid(obj);
    ll::throwSystemErrorIf(!validationResult.first, ll::ErrorCode::PortBindingError, validationResult.second);
    

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

    const auto &pushConstants = m_descriptor.getPushConstants();
    if (pushConstants.getSize() != 0) {
        vkCommandBuffer.pushConstants(m_pipelineLayout,
            vk::ShaderStageFlagBits::eCompute,
            0,
            static_cast<uint32_t>(pushConstants.getSize()),
            pushConstants.getPtr());
    }
    
    vkCommandBuffer.dispatch(m_descriptor.getGridX(),
                             m_descriptor.getGridY(),
                             m_descriptor.getGridZ());
}


void ComputeNode::onInit() {

    const auto builderName = m_descriptor.getBuilderName();
    if (!builderName.empty()) {

        // this will throw an exception if m_interpreter has been destroyed
        // by the session.
        if (auto shared_interpreter = m_interpreter.lock()) {

            constexpr const auto lua = R"(
                local builderName, node = ...
                local builder = ll.getNodeBuilder(builderName)
                builder.onNodeInit(node)
            )";

            shared_interpreter->loadAndRunNoReturn(lua, builderName, shared_from_this());

        } else {
            ll::throwSystemError(ll::ErrorCode::SessionLost, "Attempt to access the Lua interpreter of a Session already destroyed.");
        }
    }

    initPipeline();
}

void ComputeNode::bindBuffer(const ll::PortDescriptor& port, const std::shared_ptr<ll::Buffer>& buffer) {

    // holds a reference to the object
    m_objects[port.getName()] = buffer;

    // update the informacion of the descriptor set
    auto descBufferInfo = vk::DescriptorBufferInfo()
        .setOffset(0)
        .setRange(VK_WHOLE_SIZE)
        .setBuffer(buffer->m_vkBuffer);

    auto writeDescSet = vk::WriteDescriptorSet()
        .setDescriptorType(ll::portTypeToVkDescriptorType(port.getPortType()))
        .setDstSet(m_descriptorSet)
        .setDstBinding(port.getBinding())
        .setDescriptorCount(1)
        .setPBufferInfo(&descBufferInfo);

    m_device->get().updateDescriptorSets(1, &writeDescSet, 0, nullptr);
}


void ComputeNode::bindImageView(const ll::PortDescriptor& port, const std::shared_ptr<ll::ImageView>& imgView) {

    // binding
    m_objects[port.getName()] = imgView;

    auto descImgInfo = vk::DescriptorImageInfo {}
        .setSampler(imgView->m_vkSampler)
        .setImageView(imgView->m_vkImageView)
        .setImageLayout(ll::impl::toVkImageLayout(imgView->m_image->m_layout));

    auto writeDescSet = vk::WriteDescriptorSet()
        .setDstSet(m_descriptorSet)
        .setDstBinding(port.getBinding())
        .setDescriptorCount(1)
        .setPImageInfo(&descImgInfo);

    writeDescSet.setDescriptorType(ll::portTypeToVkDescriptorType(port.getPortType()));

    // update the informacion of the descriptor set
    m_device->get().updateDescriptorSets(1, &writeDescSet, 0, nullptr);
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

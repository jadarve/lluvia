#include "lluvia/core/ComputeNode.h"

#include "lluvia/core/Buffer.h"
#include "lluvia/core/ComputeNodeDescriptor.h"
#include "lluvia/core/Program.h"

#include <algorithm>
#include <iostream>

namespace ll {

using namespace std;


ComputeNode::ComputeNode(const vk::Device& device, const ll::ComputeNodeDescriptor& descriptor):
    device       {device},
    localGroup   {descriptor.getLocalGroup()},
    globalGroup  {descriptor.getGlobalGroup()},
    program      {descriptor.getProgram()},
    functionName {descriptor.getFunctionName()} {

    init(descriptor);
}


ComputeNode::~ComputeNode() {

    device.destroyPipeline(pipeline, nullptr);
    device.destroyPipelineLayout(pipelineLayout, nullptr);
    device.destroyDescriptorPool(descriptorPool, nullptr);
    device.destroyDescriptorSetLayout(descriptorSetLayout);
}


std::string ComputeNode::getFunctionName() const noexcept {
    return functionName;
}


std::shared_ptr<ll::Program> ComputeNode::getProgram() const noexcept{
    return program;
}


uint32_t ComputeNode::getLocalX() const noexcept {
    return localGroup[0];
}


uint32_t ComputeNode::getLocalY() const noexcept {
    return localGroup[1];
}


uint32_t ComputeNode::getLocalZ() const noexcept {
    return localGroup[2];
}


uint32_t ComputeNode::getGlobalX() const noexcept {
    return globalGroup[0];
}


uint32_t ComputeNode::getGlobalY() const noexcept {
    return globalGroup[1];
}


uint32_t ComputeNode::getGlobalZ() const noexcept {
    return globalGroup[2];
}


void ComputeNode::bind(int index, const std::shared_ptr<ll::Buffer> buffer) {

    // TODO: assert that index contains a buffer

    vk::DescriptorBufferInfo descBufferInfo = vk::DescriptorBufferInfo()
        .setOffset(0)
        .setRange(VK_WHOLE_SIZE)
        .setBuffer(buffer->vkBuffer);

    vk::WriteDescriptorSet writeDescSet = vk::WriteDescriptorSet()
        .setDescriptorType(vk::DescriptorType::eStorageBuffer)
        .setDstSet(descriptorSet)
        .setDstBinding(index)
        .setDescriptorCount(1)
        .setPBufferInfo(&descBufferInfo);

    // update the informacion of the descriptor set
    device.updateDescriptorSets(1, &writeDescSet, 0, nullptr);
}


void ComputeNode::record(const vk::CommandBuffer& commandBufer) const {

    commandBufer.bindPipeline(vk::PipelineBindPoint::eCompute, pipeline);
    commandBufer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
    commandBufer.dispatch(globalGroup[0], globalGroup[1], globalGroup[2]);
}


void ComputeNode::accept(ll::Visitor* visitor) {
    assert(visitor != nullptr);
}

void ComputeNode::init(const ll::ComputeNodeDescriptor& descriptor) {

    /////////////////////////////////////////////
    // Specialization constants
    /////////////////////////////////////////////

    const size_t size = sizeof(uint32_t);
    vector<vk::SpecializationMapEntry> specializationMapEntries {
        {1, 0*size, size},
        {2, 1*size, size},
        {3, 2*size, size}
    };

    vk::SpecializationInfo specializationInfo = vk::SpecializationInfo()
        .setMapEntryCount(specializationMapEntries.size())
        .setPMapEntries(specializationMapEntries.data())
        .setDataSize(localGroup.size()*sizeof(uint32_t))
        .setPData(&localGroup[0]);

    /////////////////////////////////////////////
    // Pipeline stage info
    /////////////////////////////////////////////
    stageInfo = vk::PipelineShaderStageCreateInfo()
        .setStage(vk::ShaderStageFlagBits::eCompute)
        .setModule(program->getShaderModule())
        .setPName(functionName.c_str())
        .setPSpecializationInfo(&specializationInfo);


    /////////////////////////////////////////////
    // Descriptor pool and descriptor set
    /////////////////////////////////////////////
    std::vector<vk::DescriptorSetLayoutBinding> parameterBindings {descriptor.getParameterBindings()};
    vk::DescriptorSetLayoutCreateInfo descLayoutInfo = vk::DescriptorSetLayoutCreateInfo()
        .setBindingCount(parameterBindings.size())
        .setPBindings(parameterBindings.data());

    descriptorSetLayout = device.createDescriptorSetLayout(descLayoutInfo);

    descriptorPoolSizes = descriptor.getDescriptorPoolSizes();
    descriptorPoolCreateInfo = vk::DescriptorPoolCreateInfo()
        .setMaxSets(1)
        .setPoolSizeCount(descriptorPoolSizes.size())
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


} // namespace ll

#include "lluvia/core/ComputeNode.h"

#include <algorithm>
#include <iostream>

namespace ll {

using namespace std;


ComputeNode::ComputeNode(const vk::Device& device, const ll::ComputeNodeDescriptor& descriptor):
    device {device} {

    init(descriptor);
}

ComputeNode::~ComputeNode() {

    device.destroyPipeline(pipeline, nullptr);
    device.destroyPipelineLayout(pipelineLayout, nullptr);
    device.destroyDescriptorPool(descriptorPool, nullptr);
    device.destroyDescriptorSetLayout(descriptorSetLayout);
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


void ComputeNode::record(vk::CommandBuffer& commandBufer) const {

    commandBufer.bindPipeline(vk::PipelineBindPoint::eCompute, pipeline);
    commandBufer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
    commandBufer.dispatch(globalGroup[0], globalGroup[1], globalGroup[2]);
}


void ComputeNode::init(const ll::ComputeNodeDescriptor& descriptor) {

    /////////////////////////////////////////////
    // Local and Global group
    /////////////////////////////////////////////
    localGroup = descriptor.getLocalGroup();
    globalGroup = descriptor.getGlobalGroup();

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
        .setModule(descriptor.getProgram()->getShaderModule())
        .setPName(descriptor.getFunctionName().c_str())
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

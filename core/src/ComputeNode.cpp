#include "lluvia/core/ComputeNode.h"

#include "lluvia/core/Buffer.h"
#include "lluvia/core/ComputeNodeDescriptor.h"
#include "lluvia/core/Image.h"
#include "lluvia/core/ImageView.h"
#include "lluvia/core/Object.h"
#include "lluvia/core/Program.h"

#include <algorithm>
#include <iostream>

namespace ll {

using namespace std;


ComputeNode::ComputeNode(const vk::Device& device, const ll::ComputeNodeDescriptor& descriptor):
    device       {device},
    descriptor   {descriptor} {

    init();
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


uint32_t ComputeNode::getLocalX() const noexcept {
    return descriptor.localGroup[0];
}


uint32_t ComputeNode::getLocalY() const noexcept {
    return descriptor.localGroup[1];
}


uint32_t ComputeNode::getLocalZ() const noexcept {
    return descriptor.localGroup[2];
}


uint32_t ComputeNode::getGlobalX() const noexcept {
    return descriptor.globalGroup[0];
}


uint32_t ComputeNode::getGlobalY() const noexcept {
    return descriptor.globalGroup[1];
}


uint32_t ComputeNode::getGlobalZ() const noexcept {
    return descriptor.globalGroup[2];
}


size_t ComputeNode::getParameterCount() const noexcept {
    return parameters.size();
}


std::shared_ptr<ll::Object> ComputeNode::getParameter(size_t index) const noexcept {
    assert(index < parameters.size());
    return parameters[index];
}


// void ComputeNode::bind(uint32_t index, const std::shared_ptr<ll::Buffer> buffer) {

//     // TODO: assert that index contains a buffer
//     assert(index < parameters.size());

//     parameters[index] = buffer;

//     vk::DescriptorBufferInfo descBufferInfo = vk::DescriptorBufferInfo()
//         .setOffset(0)
//         .setRange(VK_WHOLE_SIZE)
//         .setBuffer(buffer->vkBuffer);

//     vk::WriteDescriptorSet writeDescSet = vk::WriteDescriptorSet()
//         .setDescriptorType(vk::DescriptorType::eStorageBuffer)
//         .setDstSet(descriptorSet)
//         .setDstBinding(index)
//         .setDescriptorCount(1)
//         .setPBufferInfo(&descBufferInfo);

//     // update the informacion of the descriptor set
//     device.updateDescriptorSets(1, &writeDescSet, 0, nullptr);
// }


void ComputeNode::bind(uint32_t index, const std::shared_ptr<ll::Object> obj) {

    // TODO: assert that the type in obj is compatible with the descriptor at index
    assert(index < parameters.size());

    switch (obj->getType()) {
        case ll::ObjectType::Buffer:
            bindBuffer(index, std::static_pointer_cast<ll::Buffer>(obj));
            break;

        case ll::ObjectType::ImageView:
            bindImageView(index, std::static_pointer_cast<ll::ImageView>(obj));
            break;

        default:
            throw std::runtime_error("Unsupported object type: " + ll::objectTypeToString(obj->getType()));
    }

}


void ComputeNode::record(const vk::CommandBuffer& commandBufer) const {

    commandBufer.bindPipeline(vk::PipelineBindPoint::eCompute, pipeline);
    commandBufer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
    commandBufer.dispatch(descriptor.globalGroup[0], descriptor.globalGroup[1], descriptor.globalGroup[2]);
}


void ComputeNode::accept(ll::Visitor* visitor) {
    assert(visitor != nullptr);
}


void ComputeNode::init() {

    assert(descriptor.program != nullptr);
    assert(!descriptor.functionName.empty());

    parameters.resize(descriptor.parameterBindings.size());

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
        .setMapEntryCount(specializationMapEntries.size())
        .setPMapEntries(specializationMapEntries.data())
        .setDataSize(descriptor.localGroup.size()*sizeof(uint32_t))
        .setPData(&descriptor.localGroup[0]);

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
        .setBindingCount(descriptor.parameterBindings.size())
        .setPBindings(descriptor.parameterBindings.data());

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


void ComputeNode::bindBuffer(uint32_t index, const std::shared_ptr<ll::Buffer> buffer) {

    parameters[index] = buffer;

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


void ComputeNode::bindImageView(uint32_t index, const std::shared_ptr<ll::ImageView> imgView) {

    parameters[index] = imgView;

    auto descImgInfo = vk::DescriptorImageInfo {}
        .setSampler(imgView->vkSampler)
        .setImageView(imgView->vkImageView)
        .setImageLayout(imgView->image->vkLayout);

    auto writeDescSet = vk::WriteDescriptorSet()
        .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
        .setDstSet(descriptorSet)
        .setDstBinding(index)
        .setDescriptorCount(1)
        .setPImageInfo(&descImgInfo);

    // update the informacion of the descriptor set
    device.updateDescriptorSets(1, &writeDescSet, 0, nullptr);
}


} // namespace ll

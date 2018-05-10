#ifndef LLUVIA_CORE_COMPUTE_NODE_H_
#define LLUVIA_CORE_COMPUTE_NODE_H_

#include "lluvia/core/ComputeNodeDescriptor.h"

#include <cstdint>
#include <memory>
#include <string>

#include <vulkan/vulkan.hpp>

namespace ll {

class Buffer;
class Image;
class ImageView;
class Object;
class Program;
class Visitor;

/**
 * \brief Compute node.
 */
class ComputeNode {

public:
    ComputeNode()                                     = delete;
    ComputeNode(const ComputeNode& node)              = delete;
    ComputeNode(ComputeNode&& node)                   = delete;

    ComputeNode(const vk::Device& device, const ll::ComputeNodeDescriptor& descriptor);

    ~ComputeNode();

    ComputeNode& operator = (const ComputeNode& node) = delete;
    ComputeNode& operator = (ComputeNode&& node)      = delete;


    std::string getFunctionName() const noexcept;

    std::shared_ptr<ll::Program> getProgram() const noexcept;

    uint32_t getLocalX() const noexcept;
    uint32_t getLocalY() const noexcept;
    uint32_t getLocalZ() const noexcept;

    uint32_t getGridX() const noexcept;
    uint32_t getGridY() const noexcept;
    uint32_t getGridZ() const noexcept;

    size_t getParameterCount() const noexcept;
    std::shared_ptr<ll::Object> getParameter(size_t index) const noexcept;

    void bind(uint32_t index, const std::shared_ptr<ll::Object>& obj);

    void record(const vk::CommandBuffer& commandBufer) const;

    void accept(ll::Visitor* visitor);

private:
    void init();
    void bindBuffer(uint32_t index, const std::shared_ptr<ll::Buffer>& buffer);
    void bindImageView(uint32_t index, const std::shared_ptr<ll::ImageView>& imageView);

    vk::Device                          device;

    vk::DescriptorSetLayout             descriptorSetLayout;
    vk::PipelineShaderStageCreateInfo   stageInfo;

    std::vector<vk::DescriptorPoolSize> descriptorPoolSizes;
    vk::DescriptorPoolCreateInfo        descriptorPoolCreateInfo;

    vk::PipelineLayout                  pipelineLayout;
    vk::Pipeline                        pipeline;

    vk::DescriptorSet                   descriptorSet;
    vk::DescriptorPool                  descriptorPool;

    ll::ComputeNodeDescriptor           descriptor;

    // specialization constants
    // vk::SpecializationInfo specializationInfo;
    // std::vector<vk::SpecializationMapEntry> specializationMapEntries;
    // // uint32_t local_x {1};

    std::vector<std::shared_ptr<ll::Object>> objects;
};


} // namespace ll

#endif /* LLUVIA_CORE_COMPUTE_NODE_H_ */

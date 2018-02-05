#ifndef LLUVIA_CORE_COMPUTE_NODE_H_
#define LLUVIA_CORE_COMPUTE_NODE_H_

#include <cstdint>
#include <memory>
#include <string>

#include <vulkan/vulkan.hpp>

namespace ll {

class Buffer;
class ComputeNodeDescriptor;
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

    uint32_t getGlobalX() const noexcept;
    uint32_t getGlobalY() const noexcept;
    uint32_t getGlobalZ() const noexcept;


    void bind(int index, const std::shared_ptr<ll::Buffer> buffer);

    void record(const vk::CommandBuffer& commandBufer) const;

    void accept(ll::Visitor* visitor);

private:
    void init(const ll::ComputeNodeDescriptor& descriptor);

    vk::Device                          device;

    vk::DescriptorSetLayout             descriptorSetLayout;
    vk::PipelineShaderStageCreateInfo   stageInfo;

    std::vector<vk::DescriptorPoolSize> descriptorPoolSizes;
    vk::DescriptorPoolCreateInfo        descriptorPoolCreateInfo;

    vk::PipelineLayout                  pipelineLayout;
    vk::Pipeline                        pipeline;

    vk::DescriptorSet                   descriptorSet;
    vk::DescriptorPool                  descriptorPool;

    std::array<uint32_t, 3>             localGroup  {{1, 1, 1}};
    std::array<uint32_t, 3>             globalGroup {{1, 1, 1}};

    std::shared_ptr<ll::Program>        program;
    std::string                         functionName;

    // specialization constants
    // vk::SpecializationInfo specializationInfo;
    // std::vector<vk::SpecializationMapEntry> specializationMapEntries;
    // // uint32_t local_x {1};

    // std::shared_ptr<uint32_t> local_x;
};


} // namespace ll

#endif /* LLUVIA_CORE_COMPUTE_NODE_H_ */

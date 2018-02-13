#ifndef LLUVIA_CORE_COMPUTE_DESCRIPTOR_NODE_H_
#define LLUVIA_CORE_COMPUTE_DESCRIPTOR_NODE_H_

#include <array>
#include <memory>
#include <string>

#include <vulkan/vulkan.hpp>


namespace ll {

class ComputeNode;
class Program;

class ComputeNodeDescriptor {

public:
    ComputeNodeDescriptor()                                                     = default;
    ComputeNodeDescriptor(const ComputeNodeDescriptor& descriptor)              = default;
    ComputeNodeDescriptor(ComputeNodeDescriptor&& descriptor)                   = default;

    ~ComputeNodeDescriptor()                                                    = default;
    
    ComputeNodeDescriptor& operator = (const ComputeNodeDescriptor& descriptor) = default;
    ComputeNodeDescriptor& operator = (ComputeNodeDescriptor&& descriptor)      = default;

    ComputeNodeDescriptor& setProgram(std::shared_ptr<ll::Program> program);
    ComputeNodeDescriptor& setFunctionName(const std::string& name);
    ComputeNodeDescriptor& addBufferParameter();

    ComputeNodeDescriptor& setGlobalX(const uint32_t x) noexcept;
    ComputeNodeDescriptor& setGlobalY(const uint32_t y) noexcept;
    ComputeNodeDescriptor& setGlobalZ(const uint32_t z) noexcept;

    ComputeNodeDescriptor& setLocalX(const uint32_t x) noexcept;
    ComputeNodeDescriptor& setLocalY(const uint32_t y) noexcept;
    ComputeNodeDescriptor& setLocalZ(const uint32_t z) noexcept;

    std::vector<vk::DescriptorPoolSize> getDescriptorPoolSizes() const noexcept;

    std::shared_ptr<ll::Program> getProgram() const noexcept;

    std::string getFunctionName() const noexcept;
    
    std::vector<vk::DescriptorSetLayoutBinding> getParameterBindings() const noexcept;

    std::array<uint32_t, 3> getGlobalGroup() const noexcept;
    std::array<uint32_t, 3> getLocalGroup()  const noexcept;

    uint32_t getGlobalX() const noexcept;
    uint32_t getGlobalY() const noexcept;
    uint32_t getGlobalZ() const noexcept;

    uint32_t getLocalX() const noexcept;
    uint32_t getLocalY() const noexcept;
    uint32_t getLocalZ() const noexcept;

    uint32_t getStorageBufferCount() const noexcept;


private:
    uint32_t countDescriptorType(const vk::DescriptorType type) const noexcept;

    std::shared_ptr<ll::Program>                program;
    std::string                                 functionName;
    std::vector<vk::DescriptorSetLayoutBinding> parameterBindings;

    // local and global work group
    std::array<uint32_t, 3> localGroup  {{1, 1, 1}};
    std::array<uint32_t, 3> globalGroup {{1, 1, 1}};


friend class ComputeNode;
};


} // namespace ll

#endif /* LLUVIA_CORE_COMPUTE_DESCRIPTOR_NODE_H_ */

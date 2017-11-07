#ifndef LLUVIA_CORE_COMPUTE_DESCRIPTOR_NODE_H_
#define LLUVIA_CORE_COMPUTE_DESCRIPTOR_NODE_H_

#include <array>
#include <memory>
#include <string>

#include <vulkan/vulkan.hpp>

#include "lluvia/core/Program.h"

namespace ll {

class ComputeNodeDescriptor {

public:
    ComputeNodeDescriptor()                                                     = default;
    ComputeNodeDescriptor(const ComputeNodeDescriptor& descriptor)              = default;
    ComputeNodeDescriptor(ComputeNodeDescriptor&& descriptor)                   = default;
    ~ComputeNodeDescriptor()                                                    = default;
    ComputeNodeDescriptor& operator = (const ComputeNodeDescriptor& descriptor) = default;
    ComputeNodeDescriptor& operator = (ComputeNodeDescriptor&& descriptor)      = default;

    ComputeNodeDescriptor& setProgram(const std::shared_ptr<const ll::Program>& program);
    ComputeNodeDescriptor& setFunctionName(const std::string& name);
    ComputeNodeDescriptor& addBufferParameter();

    ComputeNodeDescriptor& setGlobalX(const uint32_t x) noexcept;
    ComputeNodeDescriptor& setGlobalY(const uint32_t y) noexcept;
    ComputeNodeDescriptor& setGlobalZ(const uint32_t z) noexcept;

    ComputeNodeDescriptor& setLocalX(const uint32_t x) noexcept;
    ComputeNodeDescriptor& setLocalY(const uint32_t y) noexcept;
    ComputeNodeDescriptor& setLocalZ(const uint32_t z) noexcept;

    std::vector<vk::DescriptorPoolSize> getDescriptorPoolSizes() const noexcept;

    std::string getFunctionName() const noexcept;

    std::array<uint32_t, 3> getGlobalGroup() const noexcept;
    std::array<uint32_t, 3> getLocalGroup()  const noexcept;

    uint32_t getGlobalX()         const noexcept;
    uint32_t getGlobalY()         const noexcept;
    uint32_t getGlobalZ()         const noexcept;

    uint32_t getLocalX()          const noexcept;
    uint32_t getLocalY()          const noexcept;
    uint32_t getLocalZ()          const noexcept;

    std::shared_ptr<const ll::Program>          getProgram()           const noexcept;
    std::vector<vk::DescriptorSetLayoutBinding> getParameterBindings() const noexcept;


private:
    std::shared_ptr<const ll::Program> program;
    std::string functionName;
    std::vector<vk::DescriptorSetLayoutBinding> parameterBindings;

    // local and global work group
    std::array<uint32_t, 3> localGroup  {{1, 1, 1}};
    std::array<uint32_t, 3> globalGroup {{1, 1, 1}};

    // counters for layout parameters
    uint32_t bufferCount {0};

};


} // namespace ll

#endif /* LLUVIA_CORE_COMPUTE_DESCRIPTOR_NODE_H_ */

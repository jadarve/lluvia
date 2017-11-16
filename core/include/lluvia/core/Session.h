#ifndef LLUVIA_CORE_SESSION_H_
#define LLUVIA_CORE_SESSION_H_

#include <vulkan/vulkan.hpp>

#include <cstdint>
#include <memory>
#include <string>
#include <tuple>
#include <vector>


namespace ll {

class Buffer;
class CommandBuffer;
class ComputeNode;
class ComputeNodeDescriptor;
class Memory;
class Program;

/**
 * \brief The Session class contains all the state required to run compute Graphs.
 */
class Session {

public:
    static std::unique_ptr<ll::Session> create();

    Session(const Session& session)              = delete;
    Session(Session&& session)                   = delete;

    ~Session();

    Session& operator = (const Session& session) = delete;
    Session& operator = (Session&& session)      = delete;

    vk::PhysicalDeviceMemoryProperties getPhysicalDeviceMemoryProperties() const;
    std::vector<vk::MemoryPropertyFlags> getSupportedMemoryFlags() const;

    std::unique_ptr<ll::Memory> createMemory(const vk::MemoryPropertyFlags flags, const uint64_t pageSize, bool exactFlagsMatch = true) const;

    std::shared_ptr<const ll::Program> createProgram(const std::string& spirvPath) const;

    std::unique_ptr<ll::ComputeNode> createComputeNode(const ll::ComputeNodeDescriptor& descriptor) const;

    std::shared_ptr<ll::CommandBuffer> createCommandBuffer() const;

    void run(const std::shared_ptr<ll::ComputeNode> node);

private:
    // Session objects should be created through factory methods
    Session();

    bool initInstance();
    bool initDevice();
    bool initQueue();
    bool initCommandPool();
    uint32_t getComputeFamilyQueueIndex();

    vk::Instance         instance;
    vk::PhysicalDevice   physicalDevice;
    vk::Device           device;
    vk::Queue            queue;
    vk::CommandPool      commandPool;
    uint32_t             computeQueueFamilyIndex;
};

} // namespace ll

#endif // LLUVIA_CORE_SESSION_H_

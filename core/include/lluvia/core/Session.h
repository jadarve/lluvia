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
class Image;
class Memory;
class Program;

/**
 * \brief The Session class contains all the state required to run compute Graphs.
 */
class Session {

public:
    static std::vector<vk::LayerProperties>     getVulkanInstanceLayerProperties();
    static std::vector<vk::ExtensionProperties> getVulkanExtensionProperties();
    
    static std::unique_ptr<ll::Session> create();

    Session(const Session& session)              = delete;
    Session(Session&& session)                   = delete;

    ~Session();

    Session& operator = (const Session& session) = delete;
    Session& operator = (Session&& session)      = delete;
    
    ///////////////////////////////////////////////////////
    // Memory Management
    ///////////////////////////////////////////////////////
    vk::PhysicalDeviceMemoryProperties getPhysicalDeviceMemoryProperties() const;
    std::vector<vk::MemoryPropertyFlags> getSupportedMemoryFlags() const;

    std::shared_ptr<ll::Memory> createMemory(const vk::MemoryPropertyFlags flags, const uint64_t pageSize, bool exactFlagsMatch = false) const;

    ///////////////////////////////////////////////////////
    // Memory Resources
    ///////////////////////////////////////////////////////
    std::shared_ptr<ll::CommandBuffer> createCommandBuffer() const;

    ///////////////////////////////////////////////////////
    // Compute Pipeline
    ///////////////////////////////////////////////////////
    std::shared_ptr<ll::Program> createProgram(const std::string& spirvPath) const;
    std::shared_ptr<ll::Program> createProgram(const std::vector<uint8_t>& spirv) const;

    std::shared_ptr<ll::ComputeNode> createComputeNode(const ll::ComputeNodeDescriptor& descriptor) const;

    ///////////////////////////////////////////////////////
    // Operations
    ///////////////////////////////////////////////////////
    void run(const std::shared_ptr<ll::ComputeNode>& node);
    void run(const std::shared_ptr<ll::CommandBuffer>& node);
    void copyBuffer(const ll::Buffer& src, const ll::Buffer& dst);

    void changeImageLayout(const std::shared_ptr<ll::Image>& image, const vk::ImageLayout newLayout);

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

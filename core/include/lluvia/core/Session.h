/**
@file       Session.h
@brief      Session class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

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
@brief      Class that contains all the state required to run compute operations on a compute device.
*/
class Session {

public:
    /**
    @brief      Gets the Vulkan instance layer properties available to this machine.
    
    @return     The Vulkan instance layer properties.
    */
    static std::vector<vk::LayerProperties>     getVulkanInstanceLayerProperties();


    /**
    @brief      Gets the Vulkan extension properties available to this machine.
    
    @return     The Vulkan extension properties.
    */
    static std::vector<vk::ExtensionProperties> getVulkanExtensionProperties();
    

    /**
    @brief      Creates a new ll::Session object.
    
    @return     A new session.
    */
    static std::unique_ptr<ll::Session> create();


    Session(const Session& session)              = delete;
    Session(Session&& session)                   = delete;

    ~Session();

    Session& operator = (const Session& session) = delete;
    Session& operator = (Session&& session)      = delete;
    
    
    /**
    @brief      Gets the physical device memory properties for this session.
    
    @return     The physical device memory properties.
    */
    vk::PhysicalDeviceMemoryProperties getPhysicalDeviceMemoryProperties() const;

    /**
    @brief      Gets the supported memory flags.

    The length of the returned vector equals the number of Vulkan memory types
    available for the physical device this session was created from.
    
    @return     The supported memory flags.
    */
    std::vector<vk::MemoryPropertyFlags> getSupportedMemoryFlags() const;


    /**
    @brief      Creates a memory.

    \b TODO Exceptions
    
    @param[in]  flags            The flags. It should be one of the values returned
                                 by ll::Session::getSupportedMemoryFlags().
    @param[in]  pageSize         The page size. The size in bytes of each page the new memory object
                                 will allocate when there is no space for creating new objects.
    @param[in]  exactFlagsMatch  The exact flags match. Tells whether or not \p flags should
                                 match exactly one of the values in ll::Session::getSupportedMemoryFlags()
                                 or if it is enough that it contains at least the \p flags bits.
    
    @return     A new ll::Memory object or nullptr if it could not be created.
    */
    std::shared_ptr<ll::Memory> createMemory(const vk::MemoryPropertyFlags flags, const uint64_t pageSize, bool exactFlagsMatch = false) const;

    
    /**
    @brief      Creates a command buffer.
    
    @return     A new ll::CommandBuffer object.
    */
    std::unique_ptr<ll::CommandBuffer> createCommandBuffer() const;

    
    /**
    @brief      Creates a program object reading a file at a given path.

    \b TODO Exceptions
    
    @param[in]  spirvPath  The path to the SPIR-V file containing the program code.
    
    @return     A new ll::Program object.

    @throws     std::ios_base::failure if there is a problem reading the file at spirvPath.
    */
    std::shared_ptr<ll::Program> createProgram(const std::string& spirvPath) const;


    /**
    @brief      Creates a program providing the SPIR-V code directly.

    \b TODO Exceptions
    
    @param[in]  spirv  The SPIR-V code.
    
    @return     A new ll::Program object.
    */
    std::shared_ptr<ll::Program> createProgram(const std::vector<uint8_t>& spirv) const;


    /**
    @brief      Creates a compute node.
    
    @param[in]  descriptor  The node's descriptor
    
    @return     A new ll::ComputeNode object.
    */
    std::shared_ptr<ll::ComputeNode> createComputeNode(const ll::ComputeNodeDescriptor& descriptor) const;


    /**
    @brief      Runs a ll::CommandBuffer.

    This is a blocking call. The host thread will wait until execution of
    this command buffer is completed.
    
    @param[in]  cmdBuffer  The command buffer.
    */
    void run(const ll::CommandBuffer& cmdBuffer);


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

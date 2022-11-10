/**
@file       Session.h
@brief      Session class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_SESSION_H_
#define LLUVIA_CORE_SESSION_H_

#include "lluvia/core/vulkan/vulkan.hpp"

#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <tuple>
#include <vector>

#include "lluvia/core/ComputeDimension.h"
#include "lluvia/core/SessionDescriptor.h"
#include "lluvia/core/device/DeviceDescriptor.h"
#include "lluvia/core/image/ImageDescriptor.h"
#include "lluvia/core/memory/MemoryPropertyFlags.h"
#include "lluvia/core/node/NodeBuilderDescriptor.h"
#include "lluvia/core/types.h"

namespace ll {

namespace vulkan {
    class Device;
    class Instance;
} // namespace vulkan

class Buffer;
class CommandBuffer;
class ComputeNode;
class ComputeNodeDescriptor;
class ContainerNode;
class ContainerNodeDescriptor;
class Duration;
class Image;
class Interpreter;
class Memory;
class Program;

/**
@brief      Class that contains all the state required to run compute operations on a compute device.
*/
class Session : public std::enable_shared_from_this<ll::Session> {

public:
    /**
    @brief      Gets the available devices.

    @return     The avaiable devices.
    */
    static std::vector<ll::DeviceDescriptor> getAvailableDevices();

    /**
    @brief      Gets the Vulkan instance layer properties available to this machine.

    @return     The Vulkan instance layer properties.
    */
    static std::vector<vk::LayerProperties> getVulkanInstanceLayerProperties();

    /**
    @brief      Gets the Vulkan extension properties available to this machine.

    @return     The Vulkan extension properties.
    */
    static std::vector<vk::ExtensionProperties> getVulkanExtensionProperties();

    /**
    @brief      Creates a new ll::Session object.

    @return     A new session.

    @throws     std::system_error with error code ll::ErrorCode::PhysicalDevicesNotFound
                if not physical devices are available.

    */
    static std::shared_ptr<ll::Session> create();

    /**
    @brief      Creates a new ll::Session object.

    @param[in]  descriptor for this session.

    @return     A new session.

    @throws     std::system_error with error code ll::ErrorCode::PhysicalDevicesNotFound
                if not physical devices are available.

    */
    static std::shared_ptr<ll::Session> create(const ll::SessionDescriptor& descriptor);

    Session(const Session& session) = delete;
    Session(Session&& session)      = delete;

    ~Session();

    Session& operator=(const Session& session) = delete;
    Session& operator=(Session&& session)      = delete;

    /**
    @brief      Returns a pointer to ll::Memory object that is HOST_LOCAL and HOST_COHERENT.

    This memory can be used to create uniform buffers to pass to shaders. The page size is
    set to 0 in order to create independent memories for each object allocated in this memory.
    This allows for each object to be mapped and stay mapped indefinetly, thus
    saving map/unmap operations.

    @return     The host memory.
     */
    std::shared_ptr<ll::Memory> getHostMemory() const noexcept;

    /**
    @brief      Gets the Lua interpreter.

    @return     The interpreter.
     */
    // const std::unique_ptr<ll::Interpreter>& getInterpreter() const noexcept;

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
    std::vector<ll::MemoryPropertyFlags> getSupportedMemoryFlags() const;

    /**
    @brief Gets the device descriptor associated to this session

    @return The device descriptor
     */
    const ll::DeviceDescriptor& getDeviceDescriptor() const noexcept;

    /**
    @brief      Determines if parameters in image descriptor are supported for image creation.

    This method tests whether or not the combination of image shape, tiling
    and usage flags is supported by the physical device.

    @param[in]  descriptor  The descriptor

    @return     True if image descriptor is supported, False otherwise.
    */
    bool isImageDescriptorSupported(const ll::ImageDescriptor& descriptor) const noexcept;

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

    @throws     std::system_error With error code ll::ErrorCode::MemoryCreationError
                                  if no memory was found that matched the requested flags.
    */
    std::shared_ptr<ll::Memory> createMemory(const ll::MemoryPropertyFlags& flags, const uint64_t pageSize, bool exactFlagsMatch = false);

    /**
    @brief      Creates a command buffer.

    @return     A new ll::CommandBuffer object.
    */
    std::unique_ptr<ll::CommandBuffer> createCommandBuffer() const;

    /**
    @brief      Creates a Duration object.

    @return     A new ll::Duration object.
    */
    std::unique_ptr<ll::Duration> createDuration() const;

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

    @throws     std::system_error With error code ll::ErrorCode::ProgramCompilationError
                                  if the supplied spirv code is empty.
    */
    std::shared_ptr<ll::Program> createProgram(const std::vector<uint8_t>& spirv) const;

    /**
     @brief      Sets a program with a given name into the registry.

     @param[in]  name     The name
     @param[in]  program  The program

     @sa ll::Session::getProgram Gets a program given its name in the registry.
     */
    void setProgram(const std::string& name, const std::shared_ptr<ll::Program>& program);

    /**
     @brief      Gets a program given its name in the registry.

     @param[in]  name  The name.

     @return     The program.

     @sa ll::Session::setProgram Sets a program with a given name into the registry.
     */
    std::shared_ptr<ll::Program> getProgram(const std::string& name) const;

    /**
    @brief      Gets the node builder descriptors currently registered.

    @return     The node builder descriptors.
    */
    std::vector<ll::NodeBuilderDescriptor> getNodeBuilderDescriptors() const;

    /**
    @brief      Creates a compute node.

    @param[in]  descriptor  The node's descriptor

    @return     A new ll::ComputeNode object.
    */
    std::shared_ptr<ll::ComputeNode> createComputeNode(const ll::ComputeNodeDescriptor& descriptor);

    /**
    @brief      Creates a compute node given its builder name.

    Builders can be registered by running Lua scripts using ll::Session::script method.

    @param[in]  builderName  The builder name.

    @return     A new ll::ComputeNode object
    */
    std::shared_ptr<ll::ComputeNode> createComputeNode(const std::string& builderName);

    /**
    @brief      Creates a compute node descriptor given its builder name.

    Builders can be registered by running Lua scripts using ll::Session::script method.

    @param[in]  builderName  The builder name.

    @return     the ll::ComputeNodeDescriptor generated by the builder.
    */
    ll::ComputeNodeDescriptor createComputeNodeDescriptor(const std::string& builderName) const;

    std::shared_ptr<ll::ContainerNode> createContainerNode(const ll::ContainerNodeDescriptor& descriptor);

    std::shared_ptr<ll::ContainerNode> createContainerNode(const std::string& builderName);

    ll::ContainerNodeDescriptor createContainerNodeDescriptor(const std::string& builderName) const;

    /**
    @brief      Runs a ll::CommandBuffer.

    This is a blocking call. The host thread will wait until execution of
    this command buffer is completed.

    @param[in]  cmdBuffer  The command buffer.
    */
    void run(const ll::CommandBuffer& cmdBuffer);

    /**
    @brief      Runs a ll::ComputeNode

    Internally, this function creates a ll::CommandBuffer using
    ll::Session::createCommandBuffer, records the execution of the
    compute node and submits to the device.

    Calling this function is equivalent to:

    @code
        auto cmdBuffer = session->createCommandBuffer();

        cmdBuffer->begin();
        cmdBuffer->run(node);
        cmdBuffer->end();

        session->run(*cmdBuffer);
    @endcode

    @param[in]  node  The node
    */
    void run(const ll::ComputeNode& node);

    /**
    @brief      Runs a ll::ContainerNode

    Internally, this function creates a ll::CommandBuffer using
    ll::Session::createCommandBuffer, records the execution of the
    container node and submits to the device.

    Calling this function is equivalent to:

    @code
        auto cmdBuffer = session->createCommandBuffer();

        cmdBuffer->begin();
        node.record(*cmdBuffer)
        cmdBuffer->end();

        session->run(*cmdBuffer);
    @endcode

    @param[in]  node  The node
    */
    void run(const ll::ContainerNode& node);

    /**
    @brief      Runs Lua script code into the session's internal interpreter

    @param[in]  code  The Lua code
    */
    void script(const std::string& code);

    /**
     * @brief      Runs a Lua script file into the the session's internal interpreter
     *
     * @param[in]  filename  The filename.
     */
    void scriptFile(const std::string& filename);

    /**
    @brief      Loads a library made of SPIR-V shader code and Lua scripts.

    @param[in]  filename  The path of the library file. It must be a valid
                          zip archive.

    @throws     std::system_error With error code ll::ErrorCode::IOError if there is
                                  some problem reading the library archive.
    */
    void loadLibrary(const std::string& filename);

    /**
    @brief      Returns the suggested local grid shape for compute nodes given the number of dimensions.

    The local shape depends on the underlying device.
    */
    ll::vec3ui getGoodComputeLocalShape(ll::ComputeDimension dimensions) const noexcept;

    /**
    @brief      Returns the help string of a given node builder.

    @param[in]  builderName  The builder name.

    @return     The help string of the builder.
    */
    std::string help(const std::string& builderName) const;

    /**
    @brief      Tells whether or not this session has triggered Vulkan warning messages.

    For this method to return a correct value, the session must be created enabling debug.

    @return     Whether or not vulkan warning messages have been received.
    */
    bool hasReceivedVulkanWarningMessages() const noexcept;

private:
    static uint32_t findComputeFamilyQueueIndex(vk::PhysicalDevice& physicalDevice);

    // Session objects should be created through factory methods
    Session(const ll::SessionDescriptor& descriptor);

    void initDescriptor();
    void initDevice();

    const ll::SessionDescriptor m_descriptor;
    ll::DeviceDescriptor        m_deviceDescriptor;

    std::shared_ptr<ll::vulkan::Instance> m_instance;
    std::shared_ptr<ll::vulkan::Device>   m_device;

    std::shared_ptr<ll::Interpreter> m_interpreter;

    std::map<std::string, std::shared_ptr<ll::Program>> m_programRegistry;

    std::once_flag              m_hostMemoryAllocate;
    std::shared_ptr<ll::Memory> m_hostMemory;
};

} // namespace ll

#endif // LLUVIA_CORE_SESSION_H_

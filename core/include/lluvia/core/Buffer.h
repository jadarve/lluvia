/**
@file       Buffer.h
@brief      Buffer class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_BUFFER_H_
#define LLUVIA_CORE_BUFFER_H_

#include "lluvia/core/MemoryAllocationInfo.h"
#include "lluvia/core/Object.h"

#include <vulkan/vulkan.hpp>

#include <memory>

namespace ll {

class CommandBuffer;
class ComputeGraph;
class ComputeNode;
class Memory;
class Session;
class Visitor;


namespace impl {

    /**
    String values for Vulkan VkBufferUsageFlagBits values.

    See @VULKAN_DOC#VkBufferUsageFlagBits for more information.
    */
    constexpr const std::array<std::tuple<const char*, vk::BufferUsageFlagBits>, 9> VkBufferUsageFlagBitsStrings {{
        {"INDEX_BUFFER"         , vk::BufferUsageFlagBits::eIndexBuffer},
        {"INDIRECT_BUFFER"      , vk::BufferUsageFlagBits::eIndirectBuffer},
        {"STORAGE_BUFFER"       , vk::BufferUsageFlagBits::eStorageBuffer},
        {"STORAGE_TEXEL_BUFFER" , vk::BufferUsageFlagBits::eStorageTexelBuffer},
        {"TRANSFER_DST"         , vk::BufferUsageFlagBits::eTransferDst},
        {"TRANSFER_SRC"         , vk::BufferUsageFlagBits::eTransferSrc},
        {"UNIFORM_BUFFER"       , vk::BufferUsageFlagBits::eUniformBuffer},
        {"UNIFORM_TEXEL_BUFFER" , vk::BufferUsageFlagBits::eUniformTexelBuffer},
        {"VERTEX_BUFFER"        , vk::BufferUsageFlagBits::eVertexBuffer},
    }};

} // namespace impl


/**
@brief      Converts from a string vector to Vulkan BufferUsageFlags.

The comparison between string values is case sensitive.

See @VULKAN_DOC#VkBufferUsageFlagBits for more information.

@param[in]  flagsVector  The flags vector. Their values must be contained
                         in impl::VkBufferUsageFlagBitsStrings.

@return     The reconstructed Vulkan BufferUsageFlags.
*/
inline vk::BufferUsageFlags vectorStringToBufferUsageFLags(const std::vector<std::string>& flagsVector) noexcept {
    return impl::vectorStringToFlags<vk::BufferUsageFlags, vk::BufferUsageFlagBits, impl::VkBufferUsageFlagBitsStrings.size(), impl::VkBufferUsageFlagBitsStrings>(flagsVector);
}


/**
@brief      Converst from Vulkan BufferUsageFlags to a vector of strings.

See @VULKAN_DOC#VkBufferUsageFlagBits for more information.

@param[in]  flags  The Vulkan flags.

@return     A vector of string values. Each element is one of impl::VkBufferUsageFlagBitsStrings
*/
inline std::vector<std::string> bufferUsageFlagsToVectorString(const vk::BufferUsageFlags flags) noexcept {
    return impl::flagsToVectorString<vk::BufferUsageFlags, vk::BufferUsageFlagBits, impl::VkBufferUsageFlagBitsStrings.size(), impl::VkBufferUsageFlagBitsStrings>(flags);
}


/**
@brief      Objects to manage raw portions of allocated memory.

Buffer objects are created by calling ll::Memory::createBuffer
on a specific memory instance.

@code
    auto session = ll::Session::create();

    // device-local memory with page size of 2048 bytes
    auto memory = session->createMemory(vk::MemoryPropertyFlagBits::eDeviceLocal, 2048);

    // creates a 128 bytes buffer
    auto buffer = memory->createBuffer(128);
@endcode

This object holds a reference to the ll::Memory it was created from,
guaranteeing the memory is not deleted before this buffer is.
Upon destruction of this object, the underlaying memory space is released
from the ll::Memory instance.
*/
class Buffer: public Object {

public:
    Buffer()                = delete;
    Buffer(const Buffer& b) = delete;
    Buffer(Buffer&& b)      = delete;

    ~Buffer();

    Buffer& operator = (const Buffer& buffer) = delete;
    Buffer& operator = (Buffer&& buffer)      = delete;

    ll::ObjectType getType() const noexcept override;

    /**
    @brief      Gets the allocation information.
    
    @return     The allocation information.
    */
    ll::MemoryAllocationInfo getAllocationInfo() const noexcept;


    /**
    @brief      Gets the size of the buffer in bytes.
    
    The size returned corresponds to the size requested by the user through
    Memory::createBuffer function. This size does not include padding or
    alignment bytes.

    @return     The size of the buffer in bytes.
    @sa         ll::Buffer::getAllocationInfo gets the allocation information.
    */
    uint64_t getSize() const noexcept;


    /**
    @brief      Gets the Vulkan buffer usage flags.

    See @VULKAN_DOC#VkBufferUsageFlagBits
    for more information.
    
    @return     The usage flags.
    */
    vk::BufferUsageFlags getUsageFlags() const noexcept;


    /**
    @brief      Determines if this buffer is mappable to host-visible memory.

    This function calls ll::Memory::isMappable from the memory this
    buffer was allocated from.
    
    @return     True if mappable, False otherwise.
    @sa         ll::Memory::isMappable Determines if this memory is mappable
    */
    bool  isMappable() const noexcept;


    /**
    @brief      Maps the memory content of this object to host-visible memory.

    The returned pointer can be used to read or write content from and to this buffer from
    the host.

    @code
        auto session = ll::Session::create();

        const auto hostMemFlags   = memflags::eHostVisible | memflags::eHostCoherent;;

        constexpr const auto bufferSize = 256u;
        auto hostMemory = session->createMemory(hostMemFlags, bufferSize, false);
        
        auto buffer1 = hostMemory->createBuffer(bufferSize);
        
        auto ptr = buffer->map();

        // read or write data using ptr
        
        // release the mapped memory
        buffer->unmap();
    @endcode

    Once the returned pointer is no longer needed, the user must call ll::Buffer::unmap
    to release the mapped object.
    
    @return     A pointer to host-visible memory for this buffer.
    @throws     std::system_error if the memory page containing this buffer has been
                previously mapped by this or any other object.
    @sa         ll::Buffer::unmap Unmaps this object from host visible memory.
    */
    void* map();


    /**
    @brief      Unmaps this object from host visible memory.

    @throws     std::system_error if the memory page containing this buffer has not been
                previously mapped.

    @sa         ll::Buffer::map Maps the memory content of this object to host-visible memory.
    */
    void  unmap();


    /**
    @brief      Accepts a visitor to this buffer.
    
    @param      visitor  The visitor
    */
    void accept(ll::Visitor* visitor);

private:
    Buffer( const vk::Buffer vkBuffer, const vk::BufferUsageFlags vkUsageFlags,
            const std::shared_ptr<ll::Memory>& memory, const ll::MemoryAllocationInfo& allocInfo,
            const uint64_t requestedSize);

    vk::Buffer                  vkBuffer;
    vk::BufferUsageFlags        vkUsageFlags;

    ll::MemoryAllocationInfo    allocInfo;

    // the size requested for the buffer. It can
    // be less than allocInfo.size due to alignment
    // and size requirements for the given memory.
    uint64_t                    requestedSize;

    // Shared pointer to the memory this buffer was created from
    // This will keep the memory alive until this buffer is deleted
    // avoiding reference to a corrupted memory location.
    std::shared_ptr<ll::Memory> memory;


friend class ll::CommandBuffer;
friend class ll::ComputeGraph;
friend class ll::ComputeNode;
friend class ll::Memory;
friend class ll::Session;
};


} // namespace ll

#endif // LLUVIA_CORE_BUFFER_H_

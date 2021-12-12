/**
@file       Buffer.h
@brief      Buffer class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_BUFFER_H_
#define LLUVIA_CORE_BUFFER_H_

#include "lluvia/core/error.h"
#include "lluvia/core/memory/Memory.h"
#include "lluvia/core/memory/MemoryAllocationInfo.h"
#include "lluvia/core/Object.h"
#include "lluvia/core/impl/enum_utils.h"

#include "lluvia/core/vulkan/vulkan.hpp"

#include <array>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>


namespace ll {

class Buffer;
class CommandBuffer;
class ComputeGraph;
class ComputeNode;
class Session;


namespace impl {

    /**
    String values for Vulkan VkBufferUsageFlagBits values.

    See @VULKAN_DOC#VkBufferUsageFlagBits for more information.
    */
    constexpr const std::array<std::tuple<const char*, vk::BufferUsageFlagBits>, 9> VkBufferUsageFlagBitsStrings {{
        std::make_tuple("IndexBuffer"        , vk::BufferUsageFlagBits::eIndexBuffer),
        std::make_tuple("IndirectBuffer"     , vk::BufferUsageFlagBits::eIndirectBuffer),
        std::make_tuple("StorageBuffer"      , vk::BufferUsageFlagBits::eStorageBuffer),
        std::make_tuple("StorageTexelBuffer" , vk::BufferUsageFlagBits::eStorageTexelBuffer),
        std::make_tuple("TransferDst"        , vk::BufferUsageFlagBits::eTransferDst),
        std::make_tuple("TransferSrc"        , vk::BufferUsageFlagBits::eTransferSrc),
        std::make_tuple("UniformBuffer"      , vk::BufferUsageFlagBits::eUniformBuffer),
        std::make_tuple("UniformTexelBuffer" , vk::BufferUsageFlagBits::eUniformTexelBuffer),
        std::make_tuple("VertexBuffer"       , vk::BufferUsageFlagBits::eVertexBuffer),
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
    @brief      Gets the memory this buffer was allocated from.
    
    @return     The memory.
    */
    const std::shared_ptr<ll::Memory>& getMemory() const noexcept;


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
    @brief      Gets the usage flags casted to an integer type.

    Please do not use this method. It's for internal use only.
    
    @return     The usage flags unsafe.
    */
    uint32_t getUsageFlagsUnsafe() const noexcept;


    /**
    @brief      Determines if this buffer is mappable to host-visible memory.

    This function calls ll::Memory::isPageMappable from the memory this
    buffer was allocated from.
    
    @return     True if mappable, False otherwise.
    @sa         ll::Memory::isPageMappable Determines if a certain memory page is mappable.
    */
    bool  isMappable() const noexcept;


    /**
    @brief      Deleter for unmapping buffers from host memory.
    */
    struct BufferMapDeleter{

        template<typename T>
        void operator ()([[maybe_unused]] T* ptr) const {
            buffer->unmap();
        }

        ll::Buffer* buffer;
    };


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
        
        {
            // mapped as a uint8_t [] array
            auto ptr = buffer->map<uint8_t[]>();

            for (auto i = 0u; i < bufferSize; ++i) {

                // print the value of ptr at i
                std::cout << ptr[i] << std::endl;
            }
            
        } // unmap buffer then ptr goes out of scope
    @endcode

    Once the returned pointer is no longer needed, the user must call ll::Buffer::unmap
    to release the mapped object.

    @warning    This buffer object needs to be kept alive during the whole
                lifetime of the returned mapped pointer. Otherwise, the behavior
                is undefined when the mapped pointer is deleted.

    @tparam     T     type of the memory mapped. It can be either a normal type
                      such as int or an array such as int[].
    
    @return     A std::unique_ptr to host-visible memory for this buffer. The buffer
                is unmapped automatically once this pointer is out of scope.
                
    @throws     std::system_error if the memory page containing this buffer has been
                previously mapped by this or any other object.
                
    @sa         ll::Buffer::isMappable Determines if this buffer is mappable to host-visible memory.
    */
    template<typename T>
    std::unique_ptr<T, ll::Buffer::BufferMapDeleter> map() {

        if (!m_memory->isPageMappable(m_allocInfo.page)) {
            throw std::system_error(createErrorCode(ll::ErrorCode::MemoryMapFailed), "memory page " + std::to_string(m_allocInfo.page) + " is currently mapped by another object or this memory cannot be mapped");
        }

        // remove array extend from T if present
        typedef typename std::conditional<std::is_array<T>::value, typename std::remove_all_extents<T>::type, T>::type baseType;

        auto ptr = m_memory->mapBuffer(*this);

        auto deleter = ll::Buffer::BufferMapDeleter {};
        deleter.buffer = this;

        return std::unique_ptr<T, ll::Buffer::BufferMapDeleter> {static_cast<baseType*>(ptr), deleter};
    }

    template<typename T>
    void mapAndSet(T&& obj) {

        const auto objSize = sizeof(obj);
        if (objSize > getSize()) {
            ll::throwSystemError(ll::ErrorCode::MemoryMapFailed, "size of input object (" + std::to_string(objSize) + " bytes) is greater than size of buffer (" + std::to_string(getSize()) + " bytes)");
        }

        auto ptr = map<std::remove_const_t<std::remove_reference_t<T>>>();

        std::memcpy(static_cast<void*>(ptr.get()), &obj, objSize);
    }


private:
    Buffer( const vk::Buffer tVkBuffer, const vk::BufferUsageFlags tVkUsageFlags,
            const std::shared_ptr<ll::Memory>& tMemory, const ll::MemoryAllocationInfo& tAllocInfo,
            const uint64_t tRequestedSize);

    void unmap();

    vk::Buffer                  m_vkBuffer;
    vk::BufferUsageFlags        m_vkUsageFlags;

    ll::MemoryAllocationInfo    m_allocInfo;

    // the size requested for the buffer. It can
    // be less than allocInfo.size due to alignment
    // and size requirements for the given memory.
    uint64_t                    m_requestedSize;

    // Shared pointer to the memory this buffer was created from
    // This will keep the memory alive until this buffer is deleted
    // avoiding reference to a corrupted memory location.
    std::shared_ptr<ll::Memory> m_memory;


friend class ll::CommandBuffer;
friend class ll::ComputeGraph;
friend class ll::ComputeNode;
friend class ll::Memory;
friend class ll::Session;
};

} // namespace ll

#endif // LLUVIA_CORE_BUFFER_H_

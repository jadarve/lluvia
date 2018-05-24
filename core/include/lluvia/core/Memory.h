/**
@file       Memory.h
@brief      Memory class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_MEMORY_H_
#define LLUVIA_CORE_MEMORY_H_


#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

#include <vulkan/vulkan.hpp>

#include "lluvia/core/impl/MemoryFreeSpaceManager.h"
#include "lluvia/core/impl/enum_utils.h"

namespace ll {

// forward declarations
class Buffer;
class Image;
class ImageDescriptor;
class Visitor;


namespace impl {

    /**
    String values for Vulkan VkMemoryPropertyFlagBits values.

    See @VULKAN_DOC#VkMemoryPropertyFlagBits for more information.
    */
    constexpr const std::array<std::tuple<const char*, vk::MemoryPropertyFlagBits>, 5> VkMemoryPropertyFlagBitsStrings {{
        std::make_tuple("DEVICE_LOCAL",     vk::MemoryPropertyFlagBits::eDeviceLocal),
        std::make_tuple("HOST_CACHED",      vk::MemoryPropertyFlagBits::eHostCached),
        std::make_tuple("HOST_COHERENT",    vk::MemoryPropertyFlagBits::eHostCoherent),
        std::make_tuple("HOST_VISIBLE",     vk::MemoryPropertyFlagBits::eHostVisible),
        std::make_tuple("LAZILY_ALLOCATED", vk::MemoryPropertyFlagBits::eLazilyAllocated),
    }};

} // namespace impl


/**
@brief      Converts from a string vector to Vulkan MemoryPropertyFlags.

The comparison between string values is case sensitive.

See @VULKAN_DOC#VkMemoryPropertyFlagBits for more information.

@param[in]  flagsVector  The flags vector. Their values must be contained
                         in impl::VkMemoryPropertyFlagBitsStrings.

@return     The reconstructed Vulkan MemoryPropertyFlags.
*/
inline vk::MemoryPropertyFlags vectorStringToMemoryPropertyFlags(const std::vector<std::string>& flagsVector) noexcept {
    return impl::vectorStringToFlags<vk::MemoryPropertyFlags, vk::MemoryPropertyFlagBits, impl::VkMemoryPropertyFlagBitsStrings.size(), impl::VkMemoryPropertyFlagBitsStrings>(flagsVector);
}


/**
@brief      Converst from Vulkan MemoryPropertyFlags to a vector of strings.

See @VULKAN_DOC#VkMemoryPropertyFlagBits for more information.

@param[in]  flags  The Vulkan flags.

@return     A vector of string values. Each element is one of impl::VkMemoryPropertyFlagBitsStrings
*/
inline std::vector<std::string> memoryPropertyFlagsToVectorString(const vk::MemoryPropertyFlags flags) noexcept {
    return impl::flagsToVectorString<vk::MemoryPropertyFlags, vk::MemoryPropertyFlagBits, impl::VkMemoryPropertyFlagBitsStrings.size(), impl::VkMemoryPropertyFlagBitsStrings>(flags);
}


/**
@brief      Vulkan heap information
*/
struct VkHeapInfo {

    /**
    Index to the vulkan memory type. See @VULKAN_DOC#VkPhysicalDeviceMemoryProperties
    */
    uint32_t typeIndex;

    /**
    Memory heap total size in bytes
    */
    uint64_t size;

    /**
    Memory properties. See @VULKAN_DOC#VkMemoryPropertyFlagBits
    */
    vk::MemoryPropertyFlags flags;

    /**
    Family queue indices this memory will be used on.
    */
    std::vector<uint32_t> familyQueueIndices;
};


/**
@brief      Class to manage allocation of objects into a specific type of memory.

Objects of this class contain a list of Vulkan memory objects. They are referred as memory page.
Within each page, the memory for objects such as ll::Buffer and ll::Image is reserved. Several
of such objets can be allocated in the same page. New pages are created on demand and managed
by this class.

ll::Memory objects are constructed by calling ll::Session::createMemory on a valid session object.

@code
    auto session = ll::Session::create();

    const auto hostMemFlags   = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;;
    const auto deviceMemFlags = vk::MemoryPropertyFlagBits::eDeviceLocal;
    
    // host-visible memory with page size equals to 512 bytes    
    auto hostMemory = session->createMemory(hostMemFlags, 512);

    // device-local memory with page size equals to 4096 bytes
    auto deviceMemory = session->createMemory(deviceMemFlags, 4096);
@endcode

\b TODO

- Explain how objects are allocated and aligned inside a memory page.

*/
class Memory : public std::enable_shared_from_this<ll::Memory> {

public:
    Memory()                     = delete;
    Memory(const Memory& memory) = delete;
    Memory(Memory&& memory)      = delete;

    /**
    @brief      Constructs a new ll::Memory object.
    
    @param[in]  device    The Vulkan device used for the construction.
    @param[in]  heapInfo  The heap information.
    @param[in]  pageSize  The page size in bytes.
    */
    Memory(const vk::Device device, const ll::VkHeapInfo& heapInfo, const uint64_t pageSize);

    ~Memory();

    Memory& operator = (const Memory& memory) = delete;
    Memory& operator = (Memory&& memory)      = delete;


    /**
    @brief      Gets the Vulkan memory property flags used to create this memory.

    See @VULKAN_DOC#VkMemoryPropertyFlagBits for more information.
    
    @return     The memory property flags.
    */
    vk::MemoryPropertyFlags getMemoryPropertyFlags() const noexcept;

    /**
    @brief      Gets the page size.
    
    @return     The page size.
    */
    uint64_t getPageSize()  const noexcept;

    /**
    @brief      Gets the number of pages used.
    
    @return     The page count.
    */
    uint32_t getPageCount() const noexcept;


    /**
    @brief      Determines if this memory is mappable to host-visible memory.
    
    @return     True if mappable, False otherwise.
    @sa         ll::Memory::isPageMappable Determines if a certain memory page is mappable.
    */
    bool isMappable() const noexcept;


    /**
    @brief      Determines if a certain memory page is mappable.

    This test checks if \p page is available to be mapped to host-memory by a given
    objects such as a ll::Buffer.
    
    @param[in]  page  The page index.
    
    @return     True if page mappable, False otherwise.
    */
    bool isPageMappable(const uint64_t page) const noexcept;


    /**
    @brief      Creates a new ll::Buffer object.

    The created buffer will hold a reference to this memory object, preventing it from destroying before
    the buffer is deleted.
    
    @param[in]  size        The size of the buffer in bytes. If \p size is greater than
                            the page size, a bigger page is created.
    @param[in]  usageFlags  The usage flags of the buffer. Defaults to:
                            `vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst`.
                            See @VULKAN_DOC#VkBufferUsageFlagBits.
    
    @throws     std::system_error if the Vulkan memory does not support allocating buffer objects.

    @return     A new ll::Buffer object.
    */
    std::shared_ptr<ll::Buffer> createBuffer(const uint64_t size,
        const vk::BufferUsageFlags usageFlags = {  vk::BufferUsageFlagBits::eStorageBuffer
                                                 | vk::BufferUsageFlagBits::eTransferSrc
                                                 | vk::BufferUsageFlagBits::eTransferDst});


    /**
    @brief      Creates a new ll::Image object.

    The created image will hold a reference to this memory object, preventing it from destroying before
    the image is deleted.
    
    @param[in]  descriptor  The image descriptor.
    @param[in]  usageFlags  The usage flags. Defaults to `vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eSampled`.
                See @VULKAN_DOC#VkBufferUsageFlagBits

    @throws     std::system_error if the Vulkan memory does not support allocating image objects.

    @return     A new ll::Image object.
    */
    std::shared_ptr<ll::Image> createImage(const ll::ImageDescriptor& descriptor,
        const vk::ImageUsageFlags usageFlags = {  vk::ImageUsageFlagBits::eStorage
                                                | vk::ImageUsageFlagBits::eSampled});
    
    /**
    @brief      Accepts a visitor to this memory.
    
    @param      visitor  The visitor
    */
    void accept(ll::Visitor* visitor);

private:
    impl::MemoryAllocationTryInfo getSuitableMemoryPage(const vk::MemoryRequirements& memRequirements);
    void releaseMemoryAllocation(const ll::MemoryAllocationInfo& allocInfo);

    void  releaseBuffer(const ll::Buffer& buffer);
    void* mapBuffer(const ll::Buffer& buffer);
    void  unmapBuffer(const ll::Buffer& buffer);

    void releaseImage(const ll::Image& image);

    vk::Device device;

    const ll::VkHeapInfo    heapInfo        {};
    const uint64_t          pageSize        {0u};

    std::vector<vk::DeviceMemory>                 memoryPages;
    std::vector<ll::impl::MemoryFreeSpaceManager> pageManagers;
    std::vector<bool>                             memoryPageMappingFlags;


friend class ll::Buffer;
friend class ll::Image;
};

} // namespace ll

#endif // LLUVIA_CORE_MEMORY_H_

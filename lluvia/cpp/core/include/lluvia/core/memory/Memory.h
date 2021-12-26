/**
@file       Memory.h
@brief      Memory class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_MEMORY_MEMORY_H_
#define LLUVIA_CORE_MEMORY_MEMORY_H_


#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

#include "lluvia/core/vulkan/vulkan.hpp"

#include "lluvia/core/buffer/BufferUsageFlags.h"
#include "lluvia/core/memory/MemoryPropertyFlags.h"
#include "lluvia/core/memory/MemoryFreeSpaceManager.h"

namespace ll {

namespace vulkan {
class Device;
} // namespace vulkan

// forward declarations
class Buffer;
class Image;
class ImageDescriptor;
class ImageView;
class ImageViewDescriptor;
class Session;


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
    ll::MemoryPropertyFlags flags;

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
    
    @param[in]  session   The session this node was created from.
    @param[in]  device    The Vulkan device used for the construction.
    @param[in]  heapInfo  The heap information.
    @param[in]  pageSize  The page size in bytes.
    */
    Memory(const std::shared_ptr<ll::vulkan::Device>& device,
           const ll::VkHeapInfo& heapInfo,
           const uint64_t pageSize);

    ~Memory();

    Memory& operator = (const Memory& memory) = delete;
    Memory& operator = (Memory&& memory)      = delete;


    /**
    @brief      Gets the Vulkan memory property flags used to create this memory.

    See @VULKAN_DOC#VkMemoryPropertyFlagBits for more information.
    
    @return     The memory property flags.
    */
    ll::MemoryPropertyFlags getMemoryPropertyFlags() const noexcept;

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
    bool isPageMappable(const uint32_t page) const noexcept;


    /**
    @brief      Creates a buffer.

    The created buffer will hold a reference to this memory object, preventing it from destroying before
    the buffer is deleted.

    The usage flags of the buffer will be set to
    `vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst`.
    See @VULKAN_DOC#VkBufferUsageFlagBits.  
    
    @param[in]  size  The size in bytes
    
    @return     { description_of_the_return_value }
    */
    std::shared_ptr<ll::Buffer> createBuffer(const uint64_t size);

    /**
    @brief      Creates a new ll::Buffer object.

    The created buffer will hold a reference to this memory object, preventing it from destroying before
    the buffer is deleted.
    
    @param[in]  size        The size of the buffer in bytes. If \p size is greater than
                            the page size, a bigger page is created.
    @param[in]  usageFlags  The usage flags of the buffer. See @VULKAN_DOC#VkBufferUsageFlagBits.
    
    @throws     std::system_error if the Vulkan memory does not support allocating buffer objects.

    @return     A new ll::Buffer object.
    */
    std::shared_ptr<ll::Buffer> createBuffer(const uint64_t size, const ll::BufferUsageFlags usageFlags);


    /**
    @brief      Creates buffer with usage flags passed as integer type.

    @warning Please do not use this method. It's for internal use only.
    
    @param[in]  size        The size
    @param[in]  usageFlags  The usage flags
    
    @throws     std::system_error if the Vulkan memory does not support allocating buffer objects.

    @return     A new ll::Buffer object.
    */
    std::shared_ptr<ll::Buffer> createBufferWithUnsafeFlags(const uint64_t size, const uint32_t usageFlags);
    

    /**
    @brief      Creates a new ll::Image object.

    The created image will hold a reference to this memory object, preventing it from destroying before
    the image is deleted.
    
    @param[in]  descriptor  The image descriptor.

    @throws     std::system_error if the Vulkan memory does not support allocating image objects.

    @throws     std::invalid_argument if the image size in the descriptor is not valid.
                                      This happens if the width, height or depth are zero
                                      or if the channel count ins not in the rante [1, 4].

    @return     A new ll::Image object.
    */
    std::shared_ptr<ll::Image> createImage(const ll::ImageDescriptor& descriptor);


    /**
    @brief      Creates an image view.

    This method is equivalent to

    @code
        auto image = memory->createImage(imgDescriptor);
        auto imageView = image->createImageView(videDescriptor);
    @endcode
    
    @param[in]  imgDescriptor   The underlying ll::Image descriptor.
    @param[in]  viewDescriptor  The image view descriptor.

    @throws     std::system_error if the Vulkan memory does not support allocating image objects.

    @throws     std::invalid_argument if the image size in the descriptor is not valid.
                                      This happens if the width, height or depth are zero
                                      or if the channel count ins not in the rante [1, 4].
    
    @return     A new ImageView with the underlying image storage created in thise memory.
    */
    std::shared_ptr<ll::ImageView> createImageView(
        const ll::ImageDescriptor& imgDescriptor,
        const ll::ImageViewDescriptor& viewDescriptor);


private:
    impl::MemoryAllocationTryInfo getSuitableMemoryPage(const vk::MemoryRequirements& memRequirements);
    void releaseMemoryAllocation(const ll::MemoryAllocationInfo& allocInfo);

    void  releaseBuffer(const ll::Buffer& buffer);
    void* mapBuffer(const ll::Buffer& buffer);
    void  unmapBuffer(const ll::Buffer& buffer);

    void releaseImage(const ll::Image& image);


    std::shared_ptr<ll::vulkan::Device> m_device;

    const ll::VkHeapInfo    m_heapInfo        {};
    const uint64_t          m_pageSize        {0u};

    std::vector<vk::DeviceMemory>                 m_memoryPages;
    std::vector<ll::impl::MemoryFreeSpaceManager> m_pageManagers;
    std::vector<bool>                             m_memoryPageMappingFlags;

friend class ll::Buffer;
friend class ll::Image;
};

} // namespace ll

#endif // LLUVIA_CORE_MEMORY_MEMORY_H_

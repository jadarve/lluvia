#ifndef LLUVIA_CORE_MEMORY_H_
#define LLUVIA_CORE_MEMORY_H_


#include <vector>
#include <memory>

#include <vulkan/vulkan.hpp>

#include "lluvia/core/impl/MemoryFreeSpaceManager.h"


namespace ll {

// forward declarations
class Buffer;
class Image;
class ImageDescriptor;
class Visitor;


struct VkHeapInfo {
    uint32_t typeIndex;
    uint64_t size;
    vk::MemoryPropertyFlags flags;
    std::vector<uint32_t> familyQueueIndices;
};


/*
 * \brief Class to manage a particular type of memory. Memory pages of the
 * given type are allocated and managed.
 */
class Memory : public std::enable_shared_from_this<ll::Memory> {

public:
    Memory()                     = delete;
    Memory(const Memory& memory) = delete;
    Memory(Memory&& memory)      = delete;

    Memory(const vk::Device device, const ll::VkHeapInfo& heapInfo, const uint64_t pageSize);

    ~Memory();

    Memory& operator = (const Memory& memory) = delete;
    Memory& operator = (Memory&& memory)      = delete;

    vk::MemoryPropertyFlags getMemoryPropertyFlags() const noexcept;
    uint64_t getPageSize()  const noexcept;
    uint32_t getPageCount() const noexcept;

    bool isMappable() const noexcept;

    std::shared_ptr<ll::Buffer> createBuffer(const uint64_t size,
        const vk::BufferUsageFlags usageFlags = {vk::BufferUsageFlagBits::eStorageBuffer
                                                 | vk::BufferUsageFlagBits::eTransferSrc
                                                 | vk::BufferUsageFlagBits::eTransferDst});

    void releaseBuffer(const ll::Buffer& buffer);
    void* mapBuffer(const ll::Buffer& buffer);
    void unmapBuffer(const ll::Buffer& buffer);

    std::shared_ptr<ll::Image> createImage(const ll::ImageDescriptor& descriptor);

    void accept(ll::Visitor* visitor);

private:

    inline void configureBuffer(vk::Buffer& vkBuffer, const MemoryAllocationInfo& allocInfo, const uint32_t pageIndex);
    
    inline std::shared_ptr<ll::Buffer> buildBuffer(const vk::Buffer vkBuffer,
        const vk::BufferUsageFlags vkUsageFlags,
        const ll::impl::MemoryAllocationTryInfo& tryInfo,
        const uint64_t requestedSize);

    vk::Device device;

    const ll::VkHeapInfo    heapInfo        {};
    const uint64_t          pageSize        {0u};

    std::vector<vk::DeviceMemory>                 memoryPages;
    std::vector<ll::impl::MemoryFreeSpaceManager> pageManagers;
};

} // namespace ll

#endif // LLUVIA_CORE_MEMORY_H_

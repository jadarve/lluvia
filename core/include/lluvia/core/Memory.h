#ifndef LLUVIA_CORE_MEMORY_H_
#define LLUVIA_CORE_MEMORY_H_


#include <vector>
#include <memory>

#include <vulkan/vulkan.hpp>

#include "lluvia/core/impl/MemoryFreeSpaceManager.h"


namespace ll {

// forward declarations
class Buffer;


struct VkHeapInfo {
    uint32_t heapIndex;
    uint64_t size;
    std::vector<uint32_t> familyQueueIndices;
};


/*
 * \brief Class to manage a particular type of memory. Memory pages of the
 * given type are allocated and managed.
 */
class Memory {

public:
    Memory()                     = delete;
    Memory(const Memory& memory) = delete;
    Memory(Memory&& memory)      = delete;

    Memory(const vk::Device device, const ll::VkHeapInfo& heapInfo, const uint64_t pageSize);

    ~Memory();

    Memory& operator = (const Memory& memory) = delete;
    Memory& operator = (Memory&& memory)      = delete;

    uint64_t getPageSize()  const noexcept;
    uint32_t getPageCount() const noexcept;

    std::shared_ptr<ll::Buffer> createBuffer(const uint64_t size);
    void releaseBuffer(const ll::Buffer& buffer);


private:

    inline void configureBuffer(vk::Buffer& vkBuffer, const MemoryAllocationInfo& allocInfo, const uint32_t pageIndex);
    inline std::shared_ptr<ll::Buffer> buildBuffer(const vk::Buffer vkBuffer, const ll::impl::MemoryAllocationTryInfo& tryInfo);

    vk::Device device;

    const ll::VkHeapInfo    heapInfo        {};
    const uint64_t          pageSize        {0u};

    std::vector<vk::DeviceMemory>                 memoryPages;
    std::vector<ll::impl::MemoryFreeSpaceManager> pageManagers;
};

} // namespace ll

#endif // LLUVIA_CORE_MEMORY_H_

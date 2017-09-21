#ifndef LLUVIA_CORE_MEMORY_H_
#define LLUVIA_CORE_MEMORY_H_


#include <vector>
#include <memory>

#include <vulkan/vulkan.hpp>

#include "lluvia/core/MemoryAllocationInfo.h"
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
    Memory()                     = default;
    Memory(const Memory& memory) = default;
    Memory(Memory&& memory)      = default;

    Memory(const vk::Device device, const ll::VkHeapInfo& heapInfo, const uint64_t pageSize);

    ~Memory();

    Memory& operator = (const Memory& memory) = default;
    Memory& operator = (Memory&& memory)      = default;

    uint64_t capacity() const;

    ll::Buffer allocateBuffer(const uint64_t size);
    void releaseBuffer(const ll::Buffer& buffer);


private:

    inline void configureBuffer(vk::Buffer& vkBuffer, const MemoryAllocationInfo& allocInfo, const uint32_t pageIndex);
    inline ll::Buffer buildBuffer(const vk::Buffer vkBuffer, const ll::impl::MemoryAllocationTryInfo& tryInfo);

    vk::Device device;

    const           ll::VkHeapInfo heapInfo;
    const uint64_t  pageSize;
    uint64_t        memoryCapacity;

    std::vector<vk::DeviceMemory> memoryPages;
    std::vector<ll::impl::MemoryFreeSpaceManager> pageManagers;

    std::shared_ptr<int> referenceCounter {nullptr};
};

} // namespace ll

#endif // LLUVIA_CORE_MEMORY_H_

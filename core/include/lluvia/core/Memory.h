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


struct HeapInfo {
    uint32_t index;
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

    Memory(const vk::Device device, const HeapInfo heapInfo, const uint64_t pageSize);

    ~Memory();

    Memory& operator = (const Memory& memory) = default;
    Memory& operator = (Memory&& memory)      = default;

    ll::Buffer allocateBuffer(const uint64_t size);
    void releaseBuffer(const ll::Buffer& buffer);

private:

    inline void configureBuffer(ll::Buffer& buffer, const MemoryAllocationInfo& allocInfo, const uint32_t pageIndex);

    vk::Device device;
    
    ll::HeapInfo heapInfo;
    uint64_t pageSize;
    uint64_t memoryUsed;

    std::vector<vk::DeviceMemory> memoryPages;
    std::vector<ll::impl::MemoryFreeSpaceManager> pageManagers;

    std::shared_ptr<int> referenceCounter {nullptr};

};

} // namespace ll

#endif // LLUVIA_CORE_MEMORY_H_

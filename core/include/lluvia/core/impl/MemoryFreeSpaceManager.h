#ifndef LLUVIA_CORE_IMPL_MEMORY_FREE_SPACE_MANAGER_H_
#define LLUVIA_CORE_IMPL_MEMORY_FREE_SPACE_MANAGER_H_


#include <vector>
#include <tuple>
#include <memory>

#include <vulkan/vulkan.hpp>

#include "lluvia/core/MemoryAllocationInfo.h"

namespace ll {
namespace impl {


struct MemoryAllocationTryInfo {
    ll::MemoryAllocationInfo allocInfo;
    uint32_t index;
    bool needOffsetAlignment;
    uint64_t alignmentSize;;
};


class MemoryFreeSpaceManager {

public:
    MemoryFreeSpaceManager()                                = default;
    MemoryFreeSpaceManager(const MemoryFreeSpaceManager& m) = default;
    MemoryFreeSpaceManager(MemoryFreeSpaceManager&& m)      = default;
    
    MemoryFreeSpaceManager(const uint64_t size);

    MemoryFreeSpaceManager& operator = (const MemoryFreeSpaceManager& heap) = default;
    MemoryFreeSpaceManager& operator = (MemoryFreeSpaceManager&& heap)      = default;

    uint64_t getSize() const;
    int getFreeSpaceCount() const;

    std::vector<uint64_t> getOffsetVector() const;
    std::vector<uint64_t> getSizeVector() const;

    bool allocate(uint64_t size, ll::MemoryAllocationInfo& out) noexcept;
    bool allocate(uint64_t size, uint64_t alignment, ll::MemoryAllocationInfo& out) noexcept;
    void release(const ll::MemoryAllocationInfo& info);

    bool reserveManagerSpace() noexcept;
    bool tryAllocate(uint64_t size, ll::impl::MemoryAllocationTryInfo& tryInfo) noexcept;
    bool tryAllocate(uint64_t size, uint64_t alignment, ll::impl::MemoryAllocationTryInfo& tryInfo) noexcept;
    void commitAllocation(const ll::impl::MemoryAllocationTryInfo& tyrInfo) noexcept;

private:
    uint64_t size {0};

    // separate offset and size vectors help to keep data locality
    // when scanning for inserting or deleting a new interval.
    std::vector<uint64_t> offsetVector;
    std::vector<uint64_t> sizeVector;
    
};


} // namespace impl
} // namespace ll

#endif // LLUVIA_CORE_IMPL_MEMORY_FREE_SPACE_MANAGER_H_

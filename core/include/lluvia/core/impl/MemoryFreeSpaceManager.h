#ifndef LLUVIA_CORE_IMPL_MEMORY_FREE_SPACE_MANAGER_H_
#define LLUVIA_CORE_IMPL_MEMORY_FREE_SPACE_MANAGER_H_


#include <vector>
#include <tuple>
#include <memory>

#include <vulkan/vulkan.hpp>


namespace ll {
namespace impl {


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

    std::tuple<bool, uint64_t> allocate(const uint64_t size);
    void release(const uint64_t offset, const uint64_t size);


private:

    // separate offset and size vectors help to keep data locality
    // when scanning for inserting or deleting a new interval.
    std::vector<uint64_t> offsetVector;
    std::vector<uint64_t> sizeVector;

    uint64_t size {0};
};


} // namespace impl
} // namespace ll

#endif // LLUVIA_CORE_IMPL_MEMORY_FREE_SPACE_MANAGER_H_

#ifndef LLUVIA_CORE_IMPL_MEMORY_HEAP_H_
#define LLUVIA_CORE_IMPL_MEMORY_HEAP_H_


#include <vector>
#include <tuple>
#include <memory>

#include <vulkan/vulkan.hpp>


namespace ll {
namespace impl {


class MemoryHeap {

public:
    MemoryHeap()                    = default;
    MemoryHeap(const MemoryHeap& m) = default;
    MemoryHeap(MemoryHeap&& m)      = default;
    
    MemoryHeap( const std::shared_ptr<vk::Device>& device,
                const vk::DeviceMemory& memory, const size_t size);

    ~MemoryHeap();

    MemoryHeap& operator = (const MemoryHeap& heap) = default;
    MemoryHeap& operator = (MemoryHeap&& heap)      = default;

    size_t getSize() const;
    int getFreeSpaceCount() const;

    std::tuple<bool, uint64_t> allocate(const uint64_t size);
    void release(const uint64_t offset, const uint64_t size);


private:
    vk::DeviceMemory memory;
    size_t size {0};

    // stores the offsets and sizes of each
    // contiguous block of free memory
    std::vector<uint64_t> offsetVector;
    std::vector<uint64_t> sizeVector;

    std::shared_ptr<vk::Device> device {nullptr};

    std::shared_ptr<int> referenceCounter {nullptr};
};


} // namespace impl
} // namespace ll

#endif // LLUVIA_CORE_IMPL_MEMORY_HEAP_H_

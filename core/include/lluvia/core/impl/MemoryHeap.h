#ifndef LLUVIA_CORE_IMPL_MEMORY_HEAP_H_
#define LLUVIA_CORE_IMPL_MEMORY_HEAP_H_


#include <vector>
#include <tuple>
#include <memory>

#include <vulkan/vulkan.hpp>

#include "lluvia/core/impl/MemoryFreeSpaceManager.h"

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


private:
    vk::DeviceMemory memory;

    ll::impl::MemoryFreeSpaceManager freeSpaceManager;

    // It can be a raw pointer. MemoryHeap does not own the device
    std::shared_ptr<vk::Device> device {nullptr};

    std::shared_ptr<int> referenceCounter {nullptr};
};


} // namespace impl
} // namespace ll

#endif // LLUVIA_CORE_IMPL_MEMORY_HEAP_H_

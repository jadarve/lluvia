#include "lluvia/core/impl/MemoryHeap.h"


namespace ll {
namespace impl {


MemoryHeap::MemoryHeap( const std::shared_ptr<vk::Device>& device,
                        const vk::DeviceMemory& memory, const size_t size) :
        memory {memory},
        device {device} {

    referenceCounter = std::make_shared<int>(0);

}

MemoryHeap::~MemoryHeap() {

    if(referenceCounter.use_count() == 1) {
        device->freeMemory(memory);
    }
}


} // namespace impl
} // namespace ll
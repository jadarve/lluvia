#include "lluvia/core/Buffer.h"


namespace ll {


Buffer::Buffer( const std::shared_ptr<vk::Device>& device,
                const std::shared_ptr<ll::impl::MemoryHeap>& heap,
                const uint64_t offset, const uint64_t size):
    offset {offset},
    size {size},
    device {device},
    heap {heap} {

    referenceCounter = std::make_shared<int>(0);
}


Buffer::~Buffer() {

    if(referenceCounter.use_count() == 1) {

        // FIXME: this may throw exception
        // heap->release(offset, size);
    }
}


uint64_t Buffer::getSize() const {
    return size;
}


uint64_t Buffer::getOffset() const {
    return offset;
}

}
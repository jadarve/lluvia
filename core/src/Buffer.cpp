#include "lluvia/core/Buffer.h"


namespace ll {


Buffer::Buffer( const vk::Buffer vkBuffer, ll::Memory* memory,
                const ll::MemoryAllocationInfo& allocInfo):
    vkBuffer    {vkBuffer},
    allocInfo   (allocInfo),
    memory      {memory} {

    referenceCounter = std::make_shared<int>(0);
}


Buffer::~Buffer() {

    if (referenceCounter.use_count() == 1) {
        memory->releaseBuffer(*this);
    }
}


uint64_t Buffer::getSize() const {
    return allocInfo.size;
}


} // namespace ll
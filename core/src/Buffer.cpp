#include "lluvia/core/Buffer.h"


namespace ll {


Buffer::Buffer( const vk::Buffer vkBuffer, ll::Memory* memory,
                const ll::MemoryAllocationInfo& allocInfo):
    vkBuffer         {vkBuffer},
    allocInfo        (allocInfo),
    memory           {memory} {

    assert(memory != nullptr);
}


Buffer::~Buffer() {
    memory->releaseBuffer(*this);
}


uint64_t Buffer::getSize() const {
    return allocInfo.size;
}


} // namespace ll
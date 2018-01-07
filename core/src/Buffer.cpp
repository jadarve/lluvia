#include "lluvia/core/Buffer.h"

#include "lluvia/core/Memory.h"

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


uint64_t Buffer::getSize() const noexcept {
    return allocInfo.size;
}


bool Buffer::isMappable() const noexcept {
    return memory->isMappable();
}


} // namespace ll
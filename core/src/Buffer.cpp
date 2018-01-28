#include "lluvia/core/Buffer.h"

#include "lluvia/core/Memory.h"

namespace ll {


Buffer::Buffer( const vk::Buffer vkBuffer, const vk::BufferUsageFlags vkUsageFlags,
                std::shared_ptr<ll::Memory> memory, const ll::MemoryAllocationInfo& allocInfo):
    vkBuffer         {vkBuffer},
    vkUsageFlags     {vkUsageFlags},
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


vk::BufferUsageFlags Buffer::getUsageFlags() const noexcept {
    return vkUsageFlags;
}


bool Buffer::isMappable() const noexcept {
    return memory->isMappable();
}


void Buffer::accept(ll::Visitor *visitor) {
    assert(visitor != nullptr);
}

} // namespace ll
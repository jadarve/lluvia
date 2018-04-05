#include "lluvia/core/Buffer.h"

#include "lluvia/core/Memory.h"

namespace ll {


Buffer::Buffer( const vk::Buffer vkBuffer, const vk::BufferUsageFlags vkUsageFlags,
                std::shared_ptr<ll::Memory> memory, const ll::MemoryAllocationInfo& allocInfo,
                const uint64_t requestedSize):
    vkBuffer         {vkBuffer},
    vkUsageFlags     {vkUsageFlags},
    allocInfo        (allocInfo),
    requestedSize    {requestedSize},
    memory           {memory} {

    assert(memory != nullptr);
}


Buffer::~Buffer() {
    memory->releaseBuffer(*this);
}


ll::ObjectType Buffer::getType() const noexcept {
    return ObjectType::Buffer;
}


ll::MemoryAllocationInfo Buffer::getAllocationInfo() const noexcept {
    return allocInfo;
}


uint64_t Buffer::getSize() const noexcept {
    return requestedSize;
}


vk::BufferUsageFlags Buffer::getUsageFlags() const noexcept {
    return vkUsageFlags;
}


bool Buffer::isMappable() const noexcept {
    return memory->isMappable();
}


void* Buffer::map() {
    assert(isMappable());
    
    return memory->mapBuffer(*this);
}


void Buffer::unmap() {
    memory->unmapBuffer(*this);
}


void Buffer::accept(ll::Visitor *visitor) {
    assert(visitor != nullptr);
}

} // namespace ll
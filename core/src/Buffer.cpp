/**
@file       Buffer.cpp
@brief      Buffer class and related enums.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#include "lluvia/core/Buffer.h"

#include "lluvia/core/Memory.h"

namespace ll {


Buffer::Buffer( const vk::Buffer vkBuffer, const vk::BufferUsageFlags vkUsageFlags,
                const std::shared_ptr<ll::Memory>& memory, const ll::MemoryAllocationInfo& allocInfo,
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


void Buffer::accept(ll::Visitor *visitor) {
    assert(visitor != nullptr);
}


void Buffer::unmap() {
    memory->unmapBuffer(*this);
}

} // namespace ll
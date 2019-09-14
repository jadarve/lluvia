/**
@file       Buffer.cpp
@brief      Buffer class and related enums.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#include "lluvia/core/Buffer.h"

#include "lluvia/core/Memory.h"

namespace ll {


Buffer::Buffer( const vk::Buffer tVkBuffer, const vk::BufferUsageFlags tVkUsageFlags,
                const std::shared_ptr<ll::Memory>& tMemory, const ll::MemoryAllocationInfo& tAllocInfo,
                const uint64_t tRequestedSize):
    m_vkBuffer         {tVkBuffer},
    m_vkUsageFlags     {tVkUsageFlags},
    m_allocInfo        (tAllocInfo),
    m_requestedSize    {tRequestedSize},
    m_memory           {tMemory} {
}


Buffer::~Buffer() {
    m_memory->releaseBuffer(*this);
}


ll::ObjectType Buffer::getType() const noexcept {
    return ObjectType::Buffer;
}


ll::MemoryAllocationInfo Buffer::getAllocationInfo() const noexcept {
    return m_allocInfo;
}


uint64_t Buffer::getSize() const noexcept {
    return m_requestedSize;
}


const std::shared_ptr<ll::Memory>& Buffer::getMemory() const noexcept {
    return m_memory;
}


const std::shared_ptr<ll::Session>& Buffer::getSession() const noexcept {
    return m_memory->getSession();
}


vk::BufferUsageFlags Buffer::getUsageFlags() const noexcept {
    return m_vkUsageFlags;
}


bool Buffer::isMappable() const noexcept {
    return m_memory->isMappable();
}


void Buffer::unmap() {
    m_memory->unmapBuffer(*this);
}

} // namespace ll
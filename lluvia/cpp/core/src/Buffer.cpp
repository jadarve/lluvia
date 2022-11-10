/**
@file       Buffer.cpp
@brief      Buffer class and related enums.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#include "lluvia/core/buffer/Buffer.h"

#include "lluvia/core/memory/Memory.h"

namespace ll {

Buffer::Buffer(const vk::Buffer vkBuffer, const ll::BufferUsageFlags usageFlags,
    const std::shared_ptr<ll::Memory>& memory, const ll::MemoryAllocationInfo& allocInfo,
    const uint64_t requestedSize)
    : m_vkBuffer { vkBuffer }
    , m_usageFlags { usageFlags }
    , m_allocInfo(allocInfo)
    , m_requestedSize { requestedSize }
    , m_memory { memory }
{
}

Buffer::~Buffer()
{
    m_memory->releaseBuffer(*this);
}

ll::ObjectType Buffer::getType() const noexcept
{
    return ObjectType::Buffer;
}

ll::MemoryAllocationInfo Buffer::getAllocationInfo() const noexcept
{
    return m_allocInfo;
}

uint64_t Buffer::getSize() const noexcept
{
    return m_requestedSize;
}

const std::shared_ptr<ll::Memory>& Buffer::getMemory() const noexcept
{
    return m_memory;
}

ll::BufferUsageFlags Buffer::getUsageFlags() const noexcept
{
    return m_usageFlags;
}

uint32_t Buffer::getUsageFlagsUnsafe() const noexcept
{
    return static_cast<uint32_t>(m_usageFlags);
}

bool Buffer::isMappable() const noexcept
{
    return m_memory->isMappable();
}

void Buffer::unmap()
{
    m_memory->unmapBuffer(*this);
}

} // namespace ll
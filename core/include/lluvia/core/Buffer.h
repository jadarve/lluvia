#ifndef LLUVIA_CORE_BUFFER_H_
#define LLUVIA_CORE_BUFFER_H_

#include "lluvia/core/MemoryAllocationInfo.h"
#include "lluvia/core/Object.h"

#include <vulkan/vulkan.hpp>

#include <memory>

namespace ll {

class ComputeGraph;
class ComputeNode;
class Memory;
class Session;
class Visitor;

class Buffer: public Object {

public:
    Buffer()                = delete;
    Buffer(const Buffer& b) = delete;
    Buffer(Buffer&& b)      = delete;

    ~Buffer();

    Buffer& operator = (const Buffer& buffer) = delete;
    Buffer& operator = (Buffer&& buffer)      = delete;

    ll::ObjectType getType() const noexcept override;

    uint64_t getSize() const noexcept;

    vk::BufferUsageFlags getUsageFlags() const noexcept;

    bool isMappable() const noexcept;

    void accept(ll::Visitor* visitor);

private:
    Buffer( const vk::Buffer vkBuffer, const vk::BufferUsageFlags vkUsageFlags,
            std::shared_ptr<ll::Memory> memory, const ll::MemoryAllocationInfo& allocInfo,
            const uint64_t requestedSize);

    vk::Buffer               vkBuffer;
    vk::BufferUsageFlags     vkUsageFlags;

    ll::MemoryAllocationInfo allocInfo;

    // the size requested for the buffer. It can
    // be less than allocInfo.size due to alignment
    // and size requirements for the given memory.
    uint64_t                 requestedSize;

    // Shared pointer to the memory this buffer was created from
    // This will keep the memory alive until this buffer is deleted
    // avoiding reference to a corrupted memory location.
    std::shared_ptr<ll::Memory> memory;


friend class ll::Memory;
friend class ll::ComputeGraph;
friend class ll::ComputeNode;
friend class ll::Session;
};


} // namespace ll

#endif // LLUVIA_CORE_BUFFER_H_

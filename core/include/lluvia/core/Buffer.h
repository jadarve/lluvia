#ifndef LLUVIA_CORE_BUFFER_H_
#define LLUVIA_CORE_BUFFER_H_

#include "lluvia/core/MemoryAllocationInfo.h"

#include <vulkan/vulkan.hpp>

namespace ll {

class ComputeNode;
class Memory;
class Session;

class Buffer {

public:
    Buffer()                = delete;
    Buffer(const Buffer& b) = delete;
    Buffer(Buffer&& b)      = delete;

    ~Buffer();

    Buffer& operator = (const Buffer& buffer) = delete;
    Buffer& operator = (Buffer&& buffer)      = delete;

    uint64_t getSize() const noexcept;

    bool isMappable() const noexcept;

private:
    Buffer( const vk::Buffer vkBuffer, ll::Memory* memory,
            const ll::MemoryAllocationInfo& allocInfo);

    vk::Buffer               vkBuffer;

    ll::MemoryAllocationInfo allocInfo;

    // Pointer to the Memory object this buffer was created from.
    // This pointer is guaranteed to reference a valid object since
    // construction time.
    // This buffer does not own the memory object and it is not
    // deleted in the destructor.
    ll::Memory*              memory;

friend class ll::Memory;
friend class ll::ComputeNode;
friend class ll::Session;
};


} // namespace ll

#endif // LLUVIA_CORE_BUFFER_H_

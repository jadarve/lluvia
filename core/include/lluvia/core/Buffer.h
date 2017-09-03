#ifndef LLUVIA_CORE_BUFFER_H_
#define LLUVIA_CORE_BUFFER_H_


#include <cstdint>
#include <memory>

#include <vulkan/vulkan.hpp>

#include "lluvia/core/Memory.h"

namespace ll {


class Buffer {

public:
    Buffer()                = default;
    Buffer(const Buffer& b) = default;
    Buffer(Buffer&& b)      = default;

    Buffer( ll::Memory* memory,
            const ll::MemoryAllocationInfo& allocInfo);

    virtual ~Buffer();

    Buffer& operator = (const Buffer& buffer) = default;
    Buffer& operator = (Buffer&& buffer)      = default;

    uint64_t getSize() const;

protected:
    vk::Buffer vkBuffer;

    ll::MemoryAllocationInfo allocInfo;
    ll::Memory* memory;

    std::shared_ptr<int> referenceCounter {nullptr};

friend class ll::Memory;
};


} // namespace ll

#endif // LLUVIA_CORE_BUFFER_H_

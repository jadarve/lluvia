#ifndef LLUVIA_CORE_BUFFER_H_
#define LLUVIA_CORE_BUFFER_H_


#include <cstdint>
#include <memory>

#include <vulkan/vulkan.hpp>

#include "lluvia/core/impl/MemoryHeap.h"

namespace ll {


class Buffer {

public:
    Buffer()                = default;
    Buffer(const Buffer& b) = default;
    Buffer(Buffer&& b)      = default;

    Buffer( const std::shared_ptr<vk::Device>& device,
            const std::shared_ptr<ll::impl::MemoryHeap>& heap,
            const uint64_t offset, const uint64_t size);

    virtual ~Buffer();

    Buffer& operator = (const Buffer& buffer) = default;
    Buffer& operator = (Buffer&& buffer)      = default;

    uint64_t getSize() const;
    uint64_t getOffset() const;


protected:
    uint64_t offset {0};
    uint64_t size   {0};

    std::shared_ptr<vk::Device>  device {nullptr};
    std::shared_ptr<ll::impl::MemoryHeap> heap {nullptr};

    std::shared_ptr<int> referenceCounter {nullptr};
};


} // namespace ll

#endif // LLUVIA_CORE_BUFFER_H_

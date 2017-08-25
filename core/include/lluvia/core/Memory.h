#ifndef LLUVIA_CORE_MEMORY_H_
#define LLUVIA_CORE_MEMORY_H_


#include <vector>
#include <memory>

#include <vulkan/vulkan.hpp>

#include "lluvia/core/Buffer.h"
#include "lluvia/core/impl/MemoryHeap.h"


namespace ll {


/*
 * \brief Class to manage a particular type of memory. Memory heaps of the
 * given type are allocated and managed.
 */
class Memory {

public:
    Memory()                     = default;
    Memory(const Memory& memory) = default;
    Memory(Memory&& memory)      = default;

    Memory(const uint64_t heapSize);

    ~Memory();

    Memory& operator = (const Memory& memory) = default;
    Memory& operator = (Memory&& memory)      = default;

    // ll::Buffer allocateBuffer(const uint64_t size);

private:
    std::vector<ll::impl::MemoryHeap> heaps;

};

} // namespace ll

#endif // LLUVIA_CORE_MEMORY_H_

#ifndef LLUVIA_CORE_MEMORY_ALLOCATION_H_
#define LLUVIA_CORE_MEMORY_ALLOCATION_H_

#include <cstdint>

namespace ll {

struct MemoryAllocationInfo {
    uint64_t offset;
    uint64_t size;
    uint64_t page;
};

} // namespace ll

#endif // LLUVIA_CORE_MEMORY_ALLOCATION_H_

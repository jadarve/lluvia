#ifndef LLUVIA_CORE_MEMORY_ALLOCATION_H_
#define LLUVIA_CORE_MEMORY_ALLOCATION_H_

#include <cstdint>

namespace ll {

struct MemoryAllocationInfo {

    /** \brief aligned offset */
    uint64_t offset;

    /** \brief requested size */
    uint64_t size;

    /** \brief left padding required to align offset */
    uint64_t leftPadding;

    /** \brief page number */
    uint64_t page;
};

} // namespace ll

#endif // LLUVIA_CORE_MEMORY_ALLOCATION_H_

#ifndef LLUVIA_CORE_MEMORY_ALLOCATION_INFO_H_
#define LLUVIA_CORE_MEMORY_ALLOCATION_INFO_H_

#include <cstdint>
#include <ostream>

namespace ll {

struct MemoryAllocationInfo {

    /** \brief aligned offset */
    uint64_t offset;

    /** \brief requested size */
    uint64_t size;

    /** \brief left padding required to align offset */
    uint64_t leftPadding;

    /** \brief page number within memory */
    uint64_t page;
};

} // namespace ll


// \brief print memory allocation using "[offset, size, leftPadding, page]" format.
std::ostream& operator<< (std::ostream &out, const ll::MemoryAllocationInfo& allocInfo);

#endif // LLUVIA_CORE_MEMORY_ALLOCATION_INFO_H_

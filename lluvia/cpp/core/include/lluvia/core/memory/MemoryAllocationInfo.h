/**
@file       MemoryAllocationInfo.h
@brief      Structures and methods for storing memory allocation information.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_MEMORY_MEMORY_ALLOCATION_INFO_H_
#define LLUVIA_CORE_MEMORY_MEMORY_ALLOCATION_INFO_H_

#include <cstdint>
#include <ostream>

namespace ll {

/**
@brief      Structure to hold the allocation information of objects in memory.
*/
struct MemoryAllocationInfo {

    /** @brief aligned offset */
    uint64_t offset;

    /** @brief requested size */
    uint64_t size;

    /** @brief left padding required to align offset */
    uint64_t leftPadding;

    /** @brief page number within memory */
    uint32_t page;
};

} // namespace ll

// \brief print memory allocation using "[offset, size, leftPadding, page]" format.

/**
@brief      prints the memory allocation in a output stream.

Parameter allocInfo is printed with format "[offset, size, leftPadding, page]".

@param      out        The output stream where allocInfo is printed.
@param[in]  allocInfo  The allocate information

@return     returns out parameter.
*/
std::ostream& operator<<(std::ostream& out, const ll::MemoryAllocationInfo& allocInfo);

#endif // LLUVIA_CORE_MEMORY_MEMORY_ALLOCATION_INFO_H_

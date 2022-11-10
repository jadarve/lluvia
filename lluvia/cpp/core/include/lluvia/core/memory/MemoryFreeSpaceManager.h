/**
@file       MemoryFreeSpaceManager.h
@brief      MemoryFreeSpaceManager class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_MEMORY_MEMORY_FREE_SPACE_MANAGER_H_
#define LLUVIA_CORE_MEMORY_MEMORY_FREE_SPACE_MANAGER_H_

#include <memory>
#include <ostream>
#include <tuple>
#include <vector>

#include "lluvia/core/vulkan/vulkan.hpp"

#include "lluvia/core/memory/MemoryAllocationInfo.h"

namespace ll {
namespace impl {

    struct MemoryAllocationTryInfo {
        ll::MemoryAllocationInfo allocInfo;
        uint32_t index;
    };

    class MemoryFreeSpaceManager {

    public:
        MemoryFreeSpaceManager() = default;
        MemoryFreeSpaceManager(const MemoryFreeSpaceManager& m) = default;
        MemoryFreeSpaceManager(MemoryFreeSpaceManager&& m) = default;
        MemoryFreeSpaceManager(const uint64_t tSize);

        ~MemoryFreeSpaceManager() = default;

        MemoryFreeSpaceManager& operator=(const MemoryFreeSpaceManager& heap) = default;
        MemoryFreeSpaceManager& operator=(MemoryFreeSpaceManager&& heap) = default;

        friend std::ostream& operator<<(std::ostream& out, const MemoryFreeSpaceManager& manager);

        uint64_t getSize() const noexcept;
        uint64_t getFreeSpaceCount() const noexcept;

        const std::vector<uint64_t>& getOffsetVector() const noexcept;
        const std::vector<uint64_t>& getSizeVector() const noexcept;

        bool allocate(uint64_t tSize, ll::MemoryAllocationInfo& out) noexcept;
        bool allocate(uint64_t tSize, uint64_t alignment, ll::MemoryAllocationInfo& out) noexcept;
        void release(const ll::MemoryAllocationInfo& info) noexcept;

        bool reserveManagerSpace() noexcept;
        bool tryAllocate(uint64_t tSize, ll::impl::MemoryAllocationTryInfo& tryInfoOut) noexcept;
        bool tryAllocate(uint64_t tSize, uint64_t alignment, ll::impl::MemoryAllocationTryInfo& tryInfoOut) noexcept;
        void commitAllocation(const ll::impl::MemoryAllocationTryInfo& tryInfo) noexcept;

    private:
        uint64_t m_size { 0 };

        // separate offset and size vectors help to keep data locality
        // when scanning for inserting or deleting a new interval.
        std::vector<uint64_t> m_offsetVector;
        std::vector<uint64_t> m_sizeVector;
    };

} // namespace impl
} // namespace ll

#endif // LLUVIA_CORE_MEMORY_MEMORY_FREE_SPACE_MANAGER_H_

#include "lluvia/core/impl/MemoryFreeSpaceManager.h"

#include <iostream>

namespace ll {
namespace impl {


constexpr const size_t CAPACITY_INCREASE = 512u;


MemoryFreeSpaceManager::MemoryFreeSpaceManager(const uint64_t size) :
    size            {size},
    offsetVector    {0},
    sizeVector      {size} {

    offsetVector.reserve(CAPACITY_INCREASE);
    sizeVector.reserve(CAPACITY_INCREASE);
}


std::ostream& operator << (std::ostream& out, const MemoryFreeSpaceManager& manager) {

    out << "size: " << manager.size << ". offsetVector: [size: " << manager.offsetVector.size() << ", capacity: " << manager.offsetVector.capacity() << "].";
    out << " sizeVector: [size: " << manager.sizeVector.size() << ", capacity: " << manager.sizeVector.capacity() << "]\n";

    for (auto i = 0u; i < manager.offsetVector.size(); ++i) {
        out << "    [" << manager.offsetVector[i] << ", " << manager.sizeVector[i] << "]\n";
    }
    return out;
}

uint64_t MemoryFreeSpaceManager::getSize() const noexcept {
    return size;
}


int MemoryFreeSpaceManager::getFreeSpaceCount() const noexcept {
    return offsetVector.size();
}


std::vector<uint64_t> MemoryFreeSpaceManager::getOffsetVector() const noexcept {
    return offsetVector;
}


std::vector<uint64_t> MemoryFreeSpaceManager::getSizeVector() const noexcept {
    return sizeVector;
}


bool MemoryFreeSpaceManager::allocate(uint64_t size, ll::MemoryAllocationInfo& out) noexcept {

    auto tryInfo = MemoryAllocationTryInfo{};

    if (tryAllocate(size, tryInfo)) {

        if (reserveManagerSpace()) {
            out = tryInfo.allocInfo;
            commitAllocation(tryInfo);
            return true;
        }
    }

    return false;
}


bool MemoryFreeSpaceManager::allocate(uint64_t size, uint64_t alignment, ll::MemoryAllocationInfo& out) noexcept {

    auto tryInfo = MemoryAllocationTryInfo{};

    if (tryAllocate(size, alignment, tryInfo)) {

        if (reserveManagerSpace()) {
            out = tryInfo.allocInfo;
            commitAllocation(tryInfo);
            return true;
        }
    }

    return false;
}


void MemoryFreeSpaceManager::release(const MemoryAllocationInfo& info) noexcept {

    // std::cout << "MemoryFreeSpaceManager::release: [" << info.offset << ", " << info.size << ", " << info.leftPadding << ", " << info.page << "]" << std::endl;
    // std::cout << "before:\n" << *this << std::endl;

    // correct the offset and size of the allocated interval with
    // the left padding required for aligning offset.
    auto infoLocal    = info;
    infoLocal.offset -= infoLocal.leftPadding;
    infoLocal.size   += infoLocal.leftPadding;

    const auto offsetPlusSize = infoLocal.offset + infoLocal.size;
    auto intervalUpdated   = false;
    auto lowerBoundUpdated = true;

    // update the interval offset or size if the input parameters are
    // at the boundaries of the interval.
    auto position = 0u;
    for (; position < offsetVector.size(); ++ position) {

        auto& offset_i = offsetVector[position];
        auto& size_i = sizeVector[position];

        if (offsetPlusSize == offset_i) {
            // update lower bound
            offset_i -= infoLocal.size;
            size_i   += infoLocal.size;
            intervalUpdated   = true;
            lowerBoundUpdated = true;
            break;
        }
        else if (infoLocal.offset == offset_i + size_i) {
            // update upper bound
            size_i += infoLocal.size;
            intervalUpdated   = true;
            lowerBoundUpdated = false;
            break;
        }
        // else if (infoLocal.offset > offset_i) {
        else if (offset_i > infoLocal.offset) {
            break;
        }
    }

    // std::cout << "    position: " << position << std::endl;

    // if one interval was updated, check for opportunities to merge
    // the updated interval with either the left or right one.
    if (intervalUpdated) {

        // check if there can be merges between intervals
        if (lowerBoundUpdated) {
            // std::cout << "    merging lower interval" << std::endl;

            if (position > 0) {
                const auto offsetLeft = offsetVector[position - 1];
                const auto sizeLeft = sizeVector[position - 1];

                if (offsetLeft + sizeLeft == offsetVector[position]) {

                    // merge
                    sizeVector[position - 1] += sizeVector[position];
                    offsetVector.erase(offsetVector.begin() + position);
                    sizeVector.erase(sizeVector.begin() + position);
                }
            }
        }
        else {
            // std::cout << "    merging upper interval" << std::endl;

            if (position < offsetVector.size() - 1) {

                const auto offsetRight = offsetVector[position + 1];

                if (offsetVector[position] + sizeVector[position] == offsetRight) {

                    // merge
                    offsetVector[position + 1] -= sizeVector[position];
                    sizeVector[position + 1]   += sizeVector[position];
                    offsetVector.erase(offsetVector.begin() + position);
                    sizeVector.erase(sizeVector.begin() + position);
                }
            }
        }

        // std::cout << "MemoryFreeSpaceManager::release: return end: pos: " << position << " [" << offsetVector.size() << ", " << sizeVector.size() << "]" << std::endl;
        // std::cout << *this << "\n" << std::endl;
        return;
    }

    // insert a new interval before position
    // since reserveManagerSpace() is called before inserting the new interval
    // this insert() calls should not throw exceptions.
    const auto insertOffset = offsetVector[position] > infoLocal.offset? position : std::max(0, static_cast<int32_t>(position) -1);
    // std::cout << "    insertOffset: " << insertOffset << std::endl;

    offsetVector.insert(offsetVector.begin() + insertOffset, infoLocal.offset);
    sizeVector.insert(sizeVector.begin() + insertOffset, infoLocal.size);
    // std::cout << "MemoryFreeSpaceManager::release: end: pos: " << position << " [" << offsetVector.size() << ", " << sizeVector.size() << "]" << std::endl;
    // std::cout << *this << "\n" << std::endl;
}


bool MemoryFreeSpaceManager::reserveManagerSpace() noexcept {

    // offsetVector and sizeVector should have the same size and capacity
    assert(offsetVector.size()     == sizeVector.size());
    assert(offsetVector.capacity() == sizeVector.capacity());

    try {

        if (offsetVector.size() == offsetVector.capacity()) {
            
            auto newOffsetVector = std::vector<uint64_t> {};
            auto newSizeVector   = std::vector<uint64_t> {};
            newOffsetVector.reserve(offsetVector.capacity() + CAPACITY_INCREASE);
            newSizeVector.reserve(offsetVector.capacity() + CAPACITY_INCREASE);

            newOffsetVector.insert(newOffsetVector.begin(), offsetVector.begin(), offsetVector.end());
            newSizeVector.insert(newSizeVector.begin(), sizeVector.begin(), sizeVector.end());

            // move the new vectors to the members and delete the old ones
            offsetVector = std::move(newOffsetVector);
            sizeVector   = std::move(newSizeVector);
        }

        return true;

    } catch (...) {

        // std::length_error if the new capacity of the new vectors ecceeds max_size().
        // std::bad_alloc if there is not enough memory to allocate the new vectors.
        return false;
    }
}


bool MemoryFreeSpaceManager::tryAllocate(uint64_t size, ll::impl::MemoryAllocationTryInfo& tryInfo) noexcept {

    return tryAllocate(size, 0u, tryInfo);
}


bool MemoryFreeSpaceManager::tryAllocate(uint64_t size, uint64_t alignment, ll::impl::MemoryAllocationTryInfo& tryInfo) noexcept {

    auto offsetMask  = uint64_t{0};
    auto maskCounter = alignment;

    while (maskCounter > 1u) {
        offsetMask = (offsetMask << 1) | 0x01u;
        maskCounter >>= 1;
    }

    auto position = 0;
    for (auto& s : sizeVector) {

        auto offset        = offsetVector[position];
        auto offsetModulus = offset & offsetMask;
        auto leftPadding   = (alignment - offsetModulus) & offsetMask;

        if ((size + leftPadding) <= s) {

            tryInfo.allocInfo.offset      = offset + leftPadding;
            tryInfo.allocInfo.size        = size;
            tryInfo.allocInfo.leftPadding = leftPadding;
            tryInfo.index                 = position;

            return true;
        }

        ++ position;
    }

    return false;
}


void MemoryFreeSpaceManager::commitAllocation(const ll::impl::MemoryAllocationTryInfo& tryInfo) noexcept {

    assert(tryInfo.index < offsetVector.size());
    
    // the space used for the allocation is equalt to the requested size plus
    // the bytes required to align the offset
    const auto sizePlusAlignment = tryInfo.allocInfo.size + tryInfo.allocInfo.leftPadding;

    // update offset and size of [index] block
    offsetVector[tryInfo.index] += sizePlusAlignment;
    sizeVector[tryInfo.index]   -= sizePlusAlignment;
}


} // namespace impl
} // namespace ll

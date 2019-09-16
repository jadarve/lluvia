/**
@file       MemoryFreeSpaceManager.cpp
@brief      MemoryFreeSpaceManager class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#include "lluvia/core/impl/MemoryFreeSpaceManager.h"

#include <iostream>

namespace ll {
namespace impl {


constexpr const size_t CAPACITY_INCREASE = 512u;


MemoryFreeSpaceManager::MemoryFreeSpaceManager(const uint64_t tSize) :
    m_size            {tSize},
    m_offsetVector    {0},
    m_sizeVector      {tSize} {

    m_offsetVector.reserve(CAPACITY_INCREASE);
    m_sizeVector.reserve(CAPACITY_INCREASE);
}


std::ostream& operator << (std::ostream& out, const MemoryFreeSpaceManager& manager) {

    out << "size: " << manager.m_size << ". offsetVector: [size: " << manager.m_offsetVector.size() << ", capacity: " << manager.m_offsetVector.capacity() << "].";
    out << " sizeVector: [size: " << manager.m_sizeVector.size() << ", capacity: " << manager.m_sizeVector.capacity() << "]\n";

    for (auto i = 0u; i < manager.m_offsetVector.size(); ++i) {
        out << "    [" << manager.m_offsetVector[i] << ", " << manager.m_sizeVector[i] << "]\n";
    }
    return out;
}

uint64_t MemoryFreeSpaceManager::getSize() const noexcept {
    return m_size;
}


uint64_t MemoryFreeSpaceManager::getFreeSpaceCount() const noexcept {
    return m_offsetVector.size();
}


const std::vector<uint64_t>& MemoryFreeSpaceManager::getOffsetVector() const noexcept {
    return m_offsetVector;
}


const std::vector<uint64_t>& MemoryFreeSpaceManager::getSizeVector() const noexcept {
    return m_sizeVector;
}


bool MemoryFreeSpaceManager::allocate(uint64_t tSize, ll::MemoryAllocationInfo& out) noexcept {

    auto tryInfo = MemoryAllocationTryInfo{};

    if (tryAllocate(tSize, tryInfo)) {

        if (reserveManagerSpace()) {
            out = tryInfo.allocInfo;
            commitAllocation(tryInfo);
            return true;
        }
    }

    return false;
}


bool MemoryFreeSpaceManager::allocate(uint64_t tSize, uint64_t alignment, ll::MemoryAllocationInfo& out) noexcept {

    auto tryInfo = MemoryAllocationTryInfo{};

    if (tryAllocate(tSize, alignment, tryInfo)) {

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
    for (; position < m_offsetVector.size(); ++ position) {

        auto& offset_i = m_offsetVector[position];
        auto& size_i = m_sizeVector[position];

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
                const auto offsetLeft = m_offsetVector[position - 1];
                const auto sizeLeft = m_sizeVector[position - 1];

                if (offsetLeft + sizeLeft == m_offsetVector[position]) {

                    // merge
                    m_sizeVector[position - 1] += m_sizeVector[position];
                    m_offsetVector.erase(m_offsetVector.begin() + position);
                    m_sizeVector.erase(m_sizeVector.begin() + position);
                }
            }
        }
        else {
            // std::cout << "    merging upper interval" << std::endl;

            if (position < m_offsetVector.size() - 1) {

                const auto offsetRight = m_offsetVector[position + 1];

                if (m_offsetVector[position] + m_sizeVector[position] == offsetRight) {

                    // merge
                    m_offsetVector[position + 1] -= m_sizeVector[position];
                    m_sizeVector[position + 1]   += m_sizeVector[position];
                    m_offsetVector.erase(m_offsetVector.begin() + position);
                    m_sizeVector.erase(m_sizeVector.begin() + position);
                }
            }
        }

        // std::cout << "MemoryFreeSpaceManager::release: return end: pos: " << position << " [" << m_offsetVector.size() << ", " << m_sizeVector.size() << "]" << std::endl;
        // std::cout << *this << "\n" << std::endl;
        return;
    }

    // insert a new interval before position
    // since reserveManagerSpace() is called before inserting the new interval
    // this insert() calls should not throw exceptions.
    const auto insertOffset = m_offsetVector[position] > infoLocal.offset? position : std::max(0, static_cast<int32_t>(position) -1);
    // std::cout << "    insertOffset: " << insertOffset << std::endl;

    m_offsetVector.insert(m_offsetVector.begin() + insertOffset, infoLocal.offset);
    m_sizeVector.insert(m_sizeVector.begin() + insertOffset, infoLocal.size);
    // std::cout << "MemoryFreeSpaceManager::release: end: pos: " << position << " [" << m_offsetVector.size() << ", " << m_sizeVector.size() << "]" << std::endl;
    // std::cout << *this << "\n" << std::endl;
}


bool MemoryFreeSpaceManager::reserveManagerSpace() noexcept {

    // m_offsetVector and m_sizeVector should have the same size and capacity
    assert(m_offsetVector.size()     == m_sizeVector.size());
    assert(m_offsetVector.capacity() == m_sizeVector.capacity());

    try {

        if (m_offsetVector.size() == m_offsetVector.capacity()) {
            
            auto newOffsetVector = std::vector<uint64_t> {};
            auto newSizeVector   = std::vector<uint64_t> {};
            newOffsetVector.reserve(m_offsetVector.capacity() + CAPACITY_INCREASE);
            newSizeVector.reserve(m_offsetVector.capacity() + CAPACITY_INCREASE);

            newOffsetVector.insert(newOffsetVector.begin(), m_offsetVector.begin(), m_offsetVector.end());
            newSizeVector.insert(newSizeVector.begin(), m_sizeVector.begin(), m_sizeVector.end());

            // move the new vectors to the members and delete the old ones
            m_offsetVector = std::move(newOffsetVector);
            m_sizeVector   = std::move(newSizeVector);
        }

        return true;

    } catch (...) {

        // std::length_error if the new capacity of the new vectors ecceeds max_size().
        // std::bad_alloc if there is not enough memory to allocate the new vectors.
        return false;
    }
}


bool MemoryFreeSpaceManager::tryAllocate(uint64_t tSize, ll::impl::MemoryAllocationTryInfo& tryInfoOut) noexcept {

    return tryAllocate(tSize, 0u, tryInfoOut);
}


bool MemoryFreeSpaceManager::tryAllocate(uint64_t tSize, uint64_t alignment, ll::impl::MemoryAllocationTryInfo& tryInfoOut) noexcept {

    auto offsetMask  = uint64_t{0};
    auto maskCounter = alignment;

    while (maskCounter > 1u) {
        offsetMask = (offsetMask << 1) | 0x01u;
        maskCounter >>= 1;
    }

    auto position = 0;
    for (auto& s : m_sizeVector) {

        auto offset        = m_offsetVector[position];
        auto offsetModulus = offset & offsetMask;
        auto leftPadding   = (alignment - offsetModulus) & offsetMask;

        if ((tSize + leftPadding) <= s) {

            tryInfoOut.allocInfo.offset      = offset + leftPadding;
            tryInfoOut.allocInfo.size        = tSize;
            tryInfoOut.allocInfo.leftPadding = leftPadding;
            tryInfoOut.index                 = position;

            return true;
        }

        ++ position;
    }

    return false;
}


void MemoryFreeSpaceManager::commitAllocation(const ll::impl::MemoryAllocationTryInfo& tryInfo) noexcept {

    assert(tryInfo.index < m_offsetVector.size());
    
    // the space used for the allocation is equalt to the requested size plus
    // the bytes required to align the offset
    const auto sizePlusAlignment = tryInfo.allocInfo.size + tryInfo.allocInfo.leftPadding;

    // update offset and size of [index] block
    m_offsetVector[tryInfo.index] += sizePlusAlignment;
    m_sizeVector[tryInfo.index]   -= sizePlusAlignment;
}


} // namespace impl
} // namespace ll

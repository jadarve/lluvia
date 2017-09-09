#include "lluvia/core/impl/MemoryFreeSpaceManager.h"


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


uint64_t MemoryFreeSpaceManager::getSize() const {
    return size;
}


int MemoryFreeSpaceManager::getFreeSpaceCount() const {
    return offsetVector.size();
}


std::vector<uint64_t> MemoryFreeSpaceManager::getOffsetVector() const {
    return offsetVector;
}


std::vector<uint64_t> MemoryFreeSpaceManager::getSizeVector() const {
    return sizeVector;
}


bool MemoryFreeSpaceManager::allocate(uint64_t size, ll::MemoryAllocationInfo& out) noexcept {

    auto tryInfo = MemoryAllocationTryInfo{};

    if(tryAllocate(size, tryInfo)) {
        
        if(reserveManagerSpace()) {
            out = tryInfo.allocInfo;
            commitAllocation(tryInfo);
            return true;
        }
    }

    return false;
}


bool MemoryFreeSpaceManager::allocate(uint64_t size, uint64_t alignment, ll::MemoryAllocationInfo& out) noexcept {

    auto tryInfo = MemoryAllocationTryInfo{};

    if(tryAllocate(size, alignment, tryInfo)) {
        
        if(reserveManagerSpace()) {
            out = tryInfo.allocInfo;
            commitAllocation(tryInfo);
            return true;
        }
    }

    return false;
}


void MemoryFreeSpaceManager::release(const MemoryAllocationInfo& info) {

    // correct the offset and size of the allocated interval with
    // the left padding required for aligning offset.
    auto infoLocal = info;
    infoLocal.offset -= infoLocal.leftPadding;
    infoLocal.size += infoLocal.leftPadding;
    
    auto offsetPlusSize = infoLocal.offset + infoLocal.size;
    auto intervalUpdated = false;
    auto lowerBoundUpdated = true;

    // update the interval offset or size if the input parameters are
    // at the boundaries of the interval.
    auto position = 0u;
    for(; position < offsetVector.size(); ++ position) {

        auto& offset_i = offsetVector[position];
        auto& size_i = sizeVector[position];

        if(offsetPlusSize == offset_i) {
            // update lower bound
            offset_i -= infoLocal.size;
            size_i += infoLocal.size;
            intervalUpdated = true;
            lowerBoundUpdated = true;
            break;
        }
        else if(infoLocal.offset == offset_i + size_i) {
            // update upper bound
            size_i += infoLocal.size;
            intervalUpdated = true;
            lowerBoundUpdated = false;
            break;
        }
        else if(infoLocal.offset > offset_i) {
            break;
        }
    }

    // if one interval was updated, check for opportunities to merge
    // the updated interval with either the left or right one.
    if(intervalUpdated) {

        // check if there can be merges between intervals
        if(lowerBoundUpdated) {

            if(position > 0) {
                auto offsetLeft = offsetVector[position -1];
                auto sizeLeft = sizeVector[position -1];

                if(offsetLeft + sizeLeft == offsetVector[position]) {

                    // merge
                    sizeVector[position -1] += sizeVector[position];
                    offsetVector.erase(offsetVector.begin() + position);
                    sizeVector.erase(sizeVector.begin() + position);
                }
            }
        }
        else {

            if(position < offsetVector.size() -1) {

                auto offsetRight = offsetVector[position + 1];

                if(offsetVector[position] + sizeVector[position] == offsetRight) {

                    // merge
                    offsetVector[position + 1] -= sizeVector[position];
                    sizeVector[position + 1] += sizeVector[position];
                    offsetVector.erase(offsetVector.begin() + position);
                    sizeVector.erase(sizeVector.begin() + position);
                }
            }
        }

        return;
    }

    // insert a new interval before position
    offsetVector.insert((offsetVector.begin() + position) -1, info.offset);
    sizeVector.insert((sizeVector.begin() + position) -1, info.size);
}


bool MemoryFreeSpaceManager::reserveManagerSpace() noexcept {

    try {

        // offsetVector and sizeVector should have the same size and capacity
        if(offsetVector.size() == offsetVector.capacity()) {
            auto newOffsetVector = std::vector<uint64_t> {};
            auto newSizeVector = std::vector<uint64_t> {};
            newOffsetVector.reserve(offsetVector.capacity() + CAPACITY_INCREASE);
            newSizeVector.reserve(offsetVector.capacity() + CAPACITY_INCREASE);

            newOffsetVector.insert(newOffsetVector.begin(), offsetVector.begin(), offsetVector.end());
            newSizeVector.insert(newSizeVector.begin(), sizeVector.begin(), sizeVector.end());

            // move the new vectors to the members and delete the old ones
            offsetVector = std::move(newOffsetVector);
            sizeVector = std::move(newSizeVector);
        }

        return true;

    } catch(...) {

        // std::length_error if the new capacity of the new vectors ecceeds max_size().
        // std::bad_alloc if there is not enough memory to allocate the new vectors.
        return false;
    }
}

bool MemoryFreeSpaceManager::tryAllocate(uint64_t size, ll::impl::MemoryAllocationTryInfo& tryInfo) noexcept {

    return tryAllocate(size, 0u, tryInfo);
}


bool MemoryFreeSpaceManager::tryAllocate(uint64_t size, uint64_t alignment, ll::impl::MemoryAllocationTryInfo& tryInfo) noexcept {

    auto offsetMask = uint64_t{0};
    auto maskCounter = alignment;

    while(maskCounter > 1u) {
        offsetMask = (offsetMask << 1) | 0x01u;
        maskCounter >>= 1;
    }

    auto position = 0;
    for(auto& s : sizeVector) {

    	auto offset = offsetVector[position];
    	auto offsetModulus = offset & offsetMask;
    	auto leftPadding = (alignment - offsetModulus) & offsetMask;

        if((size + leftPadding) <= s) {

            tryInfo.allocInfo.offset = offset + leftPadding;
            tryInfo.allocInfo.size = size;
            tryInfo.allocInfo.leftPadding = leftPadding;
            tryInfo.index = position;
            
            return true;
        }

        ++ position;
    }

    return false;
}


void MemoryFreeSpaceManager::commitAllocation(const ll::impl::MemoryAllocationTryInfo& tryInfo) noexcept {

    // the space used for the allocation is equalt to the requested size plus
    // the bytes required to align the offset
    auto sizePlusAlignment = tryInfo.allocInfo.size + tryInfo.allocInfo.leftPadding;

    // update offset and size of [index] block
    offsetVector[tryInfo.index] += sizePlusAlignment;
    sizeVector[tryInfo.index] -= sizePlusAlignment;    
}


} // namespace impl
} // namespace ll

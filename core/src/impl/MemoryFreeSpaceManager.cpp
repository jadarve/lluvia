#include "lluvia/core/impl/MemoryFreeSpaceManager.h"


namespace ll {
namespace impl {


MemoryFreeSpaceManager::MemoryFreeSpaceManager(const uint64_t size) :
        size            {size},
        offsetVector    {0},
        sizeVector      {size} {
        
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

    auto offsetPlusSize = info.offset + info.size;
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
            offset_i -= info.size;
            size_i += info.size;
            intervalUpdated = true;
            lowerBoundUpdated = true;
            break;
        }
        else if(info.offset == offset_i + size_i) {
            // update upper bound
            size_i += info.size;
            intervalUpdated = true;
            lowerBoundUpdated = false;
            break;
        }
        else if(info.offset > offset_i) {
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

        auto newOffsetVector = std::vector<uint64_t> {};
        auto newSizeVector = std::vector<uint64_t> {};

        // offsetVector and sizeVector should have the same size and capacity
        if(offsetVector.size() == offsetVector.capacity()) {
            newOffsetVector = std::vector<uint64_t>(offsetVector.capacity() + 1024);
            newSizeVector = std::vector<uint64_t>(sizeVector.capacity() + 1024);

            newOffsetVector.insert(newOffsetVector.begin(), offsetVector.begin(), offsetVector.end());
            newSizeVector.insert(newSizeVector.begin(), sizeVector.begin(), sizeVector.end());

            // move the new vectors to the members and delete the old ones
            offsetVector = std::move(newOffsetVector);
            sizeVector = std::move(newSizeVector);
        }

        return true;

    } catch(...) {

        // return false if any error, bad_alloc due to insufficient memory, is caught.
        return false;
    }
}

bool MemoryFreeSpaceManager::tryAllocate(uint64_t size, ll::impl::MemoryAllocationTryInfo& tryInfo) noexcept {

    return tryAllocate(size, 0u, tryInfo);
}


bool MemoryFreeSpaceManager::tryAllocate(uint64_t size, uint64_t alignment, ll::impl::MemoryAllocationTryInfo& tryInfo) noexcept {

    auto offsetMask = uint64_t{0};
    while(alignment > 0) {
        offsetMask = (offsetMask << 1) | 0x01u;
        alignment >>= 1;
    }

    auto position = 0;
    for(auto& s : sizeVector) {

        if((size + alignment) <= s) {
            
            auto offset = offsetVector[position];
            auto offsetModulus = offset & offsetMask; 

            // check if offset meets the offsetAlignment requirement
            if(offsetModulus == 0) {

                tryInfo.allocInfo.offset = offset;
                tryInfo.allocInfo.size = size;
                tryInfo.index = position;
                tryInfo.needOffsetAlignment = false;
                tryInfo.alignmentSize = 0;

            } else {

                // set parameters in tryInfo to insert a new free space interval
                tryInfo.allocInfo.offset = offset + offsetModulus;
                tryInfo.allocInfo.size = size;
                tryInfo.index = position;
                tryInfo.needOffsetAlignment = true;
                tryInfo.alignmentSize = offsetModulus;
            }

            
            return true;
        }

        ++ position;
    }

    return false;
}


void MemoryFreeSpaceManager::commitAllocation(const ll::impl::MemoryAllocationTryInfo& tryInfo) noexcept {

    // update offset and size of [index] block
    offsetVector[tryInfo.index] += tryInfo.allocInfo.size;
    sizeVector[tryInfo.index] -= tryInfo.allocInfo.size;

    if(tryInfo.needOffsetAlignment) {

        // insertion of this new interval is guaranteed to succeed as
        // the memory required for the allocation has already been reserved
        offsetVector.insert(offsetVector.begin() + tryInfo.index, tryInfo.allocInfo.offset - tryInfo.alignmentSize);
        sizeVector.insert(sizeVector.begin() + tryInfo.index, tryInfo.alignmentSize);
    }
    
}


} // namespace impl
} // namespace ll

#include "lluvia/core/impl/MemoryFreeSpaceManager.h"


namespace ll {
namespace impl {


MemoryFreeSpaceManager::MemoryFreeSpaceManager(const uint64_t size) :
        offsetVector {0},
        sizeVector {size},
        size {size} {
        
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


bool MemoryFreeSpaceManager::allocate(uint64_t size, ll::impl::MemoryAllocationInfo& out) noexcept {

    auto position = 0;
    for(auto& s : sizeVector) {

        if(size <= s) {

            auto offset = offsetVector[position];

            // update offset and size of this block
            offsetVector[position] += size;
            sizeVector[position] -= size;

            out.offset = offset;
            out.size = size;
            return true;
        }

        ++ position;
    }

    return true;
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


} // namespace impl
} // namespace ll

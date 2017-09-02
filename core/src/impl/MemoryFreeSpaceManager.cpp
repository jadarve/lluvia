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


std::tuple<bool, uint64_t> MemoryFreeSpaceManager::allocate(const uint64_t size) {

    auto position = 0;
    for(auto& s : sizeVector) {

        if(size <= s) {

            auto offset = offsetVector[position];

            // update offset and size of this block
            offsetVector[position] += size;
            sizeVector[position] -= size;

            return std::make_tuple(true, offset);
        }

        ++ position;
    }

    return std::make_tuple(false, 0);
}


void MemoryFreeSpaceManager::release(const uint64_t offset, const uint64_t size) {

    auto offsetPlusSize = offset + size;
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
            offset_i -= size;
            intervalUpdated = true;
            lowerBoundUpdated = true;
            break;
        }
        else if(offset == offset_i + size_i) {
            // update upper bound
            size_i += size;
            intervalUpdated = true;
            lowerBoundUpdated = false;
            break;
        }
        else if(offset > offset_i) {
            break;
        }
    }

    // if one interval was updated, check for opportunities to merge
    // the updated interval with either the left or right one.
    if(intervalUpdated) {

        // check if there can be merges between intervals
        if(lowerBoundUpdated) {

            if(position > 0) {
                auto offsetPosLeft = offsetVector[position -1];
                auto& sizePosLeft = sizeVector[position -1];

                if(offsetPosLeft + sizePosLeft == offsetVector[position]) {

                    // merge
                    sizePosLeft += sizeVector[position];
                    offsetVector.erase(offsetVector.begin() + position);
                    sizeVector.erase(sizeVector.begin() + position);
                }
            }
        }
        else {

            if(position < offsetVector.size() -1) {

                auto& offsetPosRight = offsetVector[position + 1];

                if(offsetVector[position] + sizeVector[position] == offsetPosRight) {

                    // merge
                    offsetPosRight -= sizeVector[position];
                    offsetVector.erase(offsetVector.begin() + position);
                    sizeVector.erase(sizeVector.begin() + position);
                }
            }
        }

        return;
    }


    // insert a new interval before position
    offsetVector.insert((offsetVector.begin() + position) -1, offset);
    sizeVector.insert((sizeVector.begin() + position) -1, size);
}


} // namespace impl
} // namespace ll

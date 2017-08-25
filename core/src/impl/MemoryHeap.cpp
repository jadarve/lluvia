#include "lluvia/core/impl/MemoryHeap.h"


namespace ll {
namespace impl {


MemoryHeap::MemoryHeap( const std::shared_ptr<vk::Device>& device,
                        const vk::DeviceMemory& memory, const size_t size) :
        memory {memory},
        size {size},
        offsetVector {0},
        sizeVector {size},
        device {device} {

    referenceCounter = std::make_shared<int>(0);

}

MemoryHeap::~MemoryHeap() {

    if(referenceCounter.use_count() == 1) {
        device->freeMemory(memory);
    }
}


size_t MemoryHeap::getSize() const {
    return size;
}


int MemoryHeap::getFreeSpaceCount() const {
    return offsetVector.size();
}


std::tuple<bool, uint64_t> MemoryHeap::allocate(const uint64_t size) {

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


void MemoryHeap::release(const uint64_t offset, const uint64_t size) {

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
    offsetVector.insert(offsetVector.begin() + position, offset);
    sizeVector.insert(sizeVector.begin() + position, size);
}


} // namespace impl
} // namespace ll
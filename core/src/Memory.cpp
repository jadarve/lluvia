#include "lluvia/core/Memory.h"

#include "lluvia/core/Buffer.h"

namespace ll {


Memory::Memory(const uint64_t pageSize):
    pageSize {pageSize} {

}


Memory::~Memory() {

}


ll::Buffer Memory::allocateBuffer(const uint64_t size) {

    auto allocInfo = MemoryAllocationInfo{};

    auto fitsInPage = false;
    auto pageIndex = 0u;
    for(auto& manager : pageManagers) {

        if(manager.allocate(size, allocInfo)) {

            allocInfo.page = pageIndex;
            fitsInPage = true;
            break;
        }

        ++ pageIndex;
    }

    
    if(fitsInPage) {
        // TODO
    }


    // None of the existing pages could allocate the buffer


    return ll::Buffer{};
}


void Memory::releaseBuffer(const ll::Buffer& buffer) {

    pageManagers[buffer.allocInfo.page].release(buffer.allocInfo);
    device.destroyBuffer(buffer.vkBuffer);
}


} // namespace ll
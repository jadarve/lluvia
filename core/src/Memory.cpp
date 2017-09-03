#include "lluvia/core/Memory.h"

#include <algorithm>

#include "lluvia/core/Buffer.h"

namespace ll {


Memory::Memory(const vk::Device device, const HeapInfo heapInfo, const uint64_t pageSize):
    device {device},
    pageSize {pageSize},
    memoryUsed {0u} {
    
    this->heapInfo = std::move(heapInfo);
}


Memory::~Memory() {

}


ll::Buffer Memory::allocateBuffer(const uint64_t size) {

    auto buffer = ll::Buffer {};

    // TODO: figure out buffer alignment
    auto sizeAligned = size;

    auto tryInfo = impl::MemoryAllocationTryInfo{};

    auto pageIndex = 0u;
    for(auto& manager : pageManagers) {

        if(manager.tryAllocate(sizeAligned, tryInfo)) {

            // Configure buffer object. Internally handles error cases.
            configureBuffer(buffer, tryInfo.allocInfo, pageIndex);
            
            // The Buffer object has been constructed successfully and it is
            // now possible to commit the allocation.
            manager.commitAllocation(tryInfo);
            return buffer;
        }

        ++ pageIndex;
    }


    // None of the existing pages could allocate the buffer. Create a ne
    // memory page and allocate the buffer in it.
    
    // FIXME: Check for the availability of more device memory and see
    // if it is possible to create a new vk::DeviceMemory object according
    // to the physical device limits.

    auto newPageSize = std::max(pageSize, sizeAligned);
    

    // reserve space to store a new memory page and manager.
    if(memoryPages.size() == memoryPages.capacity()) {
        memoryPages.reserve(memoryPages.capacity() + 1);
    }

    if(pageManagers.size() == pageManagers.capacity()) {
        pageManagers.reserve(memoryPages.capacity() + 1);
    }


    vk::MemoryAllocateInfo allocateInfo = vk::MemoryAllocateInfo()
        .setAllocationSize(newPageSize)
        .setMemoryTypeIndex(heapInfo.index);


    // Safe to not try-catch the creation of manager and memory.
    // If exception is thrown, this object is left in its previous
    // state plus the reserved space in memoryPages and pageManagers.
    auto manager = impl::MemoryFreeSpaceManager {newPageSize};
    auto memory = device.allocateMemory(allocateInfo);


    // push objects to vectors after reserving space
    memoryPages.push_back(memory);
    pageManagers.push_back(std::move(manager));


    // this allocation try is guaranteed to work as there is enough
    // free space in the page to fit sizeAligned.
    manager.tryAllocate(sizeAligned, tryInfo);
    configureBuffer(buffer, tryInfo.allocInfo, pageIndex);
    
    // The Buffer object has been constructed successfully and it is
    // now possible to commit the allocation.
    manager.commitAllocation(tryInfo);
    return buffer;
}


void Memory::releaseBuffer(const ll::Buffer& buffer) {

    pageManagers[buffer.allocInfo.page].release(buffer.allocInfo);
    device.destroyBuffer(buffer.vkBuffer);
}


inline void Memory::configureBuffer(ll::Buffer& buffer, const MemoryAllocationInfo& allocInfo,
    const uint32_t pageIndex) {

    buffer.allocInfo = allocInfo;
    buffer.allocInfo.page = pageIndex;
    buffer.memory = this;

    vk::BufferCreateInfo bufferInfo = vk::BufferCreateInfo()
        .setSharingMode(vk::SharingMode::eExclusive)
        .setSize(allocInfo.size)
        .setUsage(vk::BufferUsageFlagBits::eStorageBuffer)
        .setQueueFamilyIndexCount(heapInfo.familyQueueIndices.size())
        .setPQueueFamilyIndices(heapInfo.familyQueueIndices.data());

    // It's safe to not guard this call with a try-catch. If an
    // exception is thrown, let the caller to handle it. The memory
    // manager is still in its original state.
    buffer.vkBuffer = device.createBuffer(bufferInfo);

    try {
        device.bindBufferMemory(buffer.vkBuffer, memoryPages[pageIndex], buffer.allocInfo.offset);

    } catch(...) {

        // need to destroy the buffer before rethrowing the exception.
        device.destroyBuffer(buffer.vkBuffer);
        throw;  // rethrow exception
    }
}

} // namespace ll
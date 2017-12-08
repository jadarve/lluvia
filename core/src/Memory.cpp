#include "lluvia/core/Memory.h"

#include <algorithm>

#include "lluvia/core/Buffer.h"
#include "lluvia/core/MemoryAllocationInfo.h"

namespace ll {


Memory::Memory(const vk::Device device, const ll::VkHeapInfo& heapInfo, const uint64_t pageSize):
    device              {device},
    heapInfo            (heapInfo),
    pageSize            {pageSize} {
        
}


Memory::~Memory() {

    for (auto& memory : memoryPages) {
        device.freeMemory(memory);
    }
}


uint64_t Memory::getPageSize() const noexcept {
    return pageSize;
}

uint32_t Memory::getPageCount() const noexcept {
    return static_cast<uint32_t>(memoryPages.size());
}


std::shared_ptr<ll::Buffer> Memory::createBuffer(const uint64_t size) {

    vk::BufferCreateInfo bufferInfo = vk::BufferCreateInfo()
                                      .setSharingMode(vk::SharingMode::eExclusive)
                                      .setSize(size)
                                      .setUsage(vk::BufferUsageFlagBits::eStorageBuffer)
                                      .setQueueFamilyIndexCount(heapInfo.familyQueueIndices.size())
                                      .setPQueueFamilyIndices(heapInfo.familyQueueIndices.data());

    // It's safe to not guard this call with a try-catch. If an
    // exception is thrown, let the caller to handle it. The memory
    // manager is still in its original state.
    auto vkBuffer = device.createBuffer(bufferInfo);

    // query alignment and offset
    const auto memRequirements = device.getBufferMemoryRequirements(vkBuffer);

    auto tryInfo = impl::MemoryAllocationTryInfo{};
    auto pageIndex = 0u;
    for (auto& manager : pageManagers) {

        if (manager.tryAllocate(memRequirements.size, memRequirements.alignment, tryInfo)) {

            // reserve space in the memory manager to insert any new free interval
            if (manager.reserveManagerSpace()) {

                // Configure buffer object. Internally handles error cases.
                configureBuffer(vkBuffer, tryInfo.allocInfo, pageIndex);

                // build a ll::Buffer object and commit the allocation if the
                // object construction is successful.
                return buildBuffer(vkBuffer, tryInfo);
            }
        }

        ++ pageIndex;
    }


    // None of the existing pages could allocate the buffer. Create a ne
    // memory page and allocate the buffer in it.

    // FIXME: Check for the availability of more device memory and see
    // if it is possible to create a new vk::DeviceMemory object according
    // to the physical device limits.

    const auto newPageSize = std::max(pageSize, memRequirements.size);


    // reserve space to store a new memory page and manager.
    if (memoryPages.size() == memoryPages.capacity()) {
        memoryPages.reserve(memoryPages.capacity() + 1);
    }

    if (pageManagers.size() == pageManagers.capacity()) {
        pageManagers.reserve(pageManagers.capacity() + 1);
    }


    vk::MemoryAllocateInfo allocateInfo = vk::MemoryAllocateInfo()
                                          .setAllocationSize(newPageSize)
                                          .setMemoryTypeIndex(heapInfo.heapIndex);


    // Safe to not try-catch the creation of manager and memory.
    // If exception is thrown, this object is left in its previous
    // state plus the reserved space in memoryPages and pageManagers.
    auto manager = impl::MemoryFreeSpaceManager {newPageSize};
    manager.reserveManagerSpace();

    auto memory  = device.allocateMemory(allocateInfo);

    // push objects to vectors after reserving space
    memoryPages.push_back(memory);
    pageManagers.push_back(std::move(manager));
    
    // this allocation try is guaranteed to work as there is enough
    // free space in the page to fit memRequirements.size.
    pageManagers[pageIndex].tryAllocate(memRequirements.size, memRequirements.alignment, tryInfo);
    configureBuffer(vkBuffer, tryInfo.allocInfo, pageIndex);

    // build a ll::Buffer object and commit the allocation if the
    // object construction is successful.
    return buildBuffer(vkBuffer, tryInfo);
}


void Memory::releaseBuffer(const ll::Buffer& buffer) {

    // reserve space in case the release of this buffer requires
    // the insertion of a new free interval.
    pageManagers[buffer.allocInfo.page].reserveManagerSpace();

    // release the allocated memory regardless of the result of
    // reserveManagerSpace(). In case it failed the release method
    // should throw and exception and abort the program (since it is
    // declared as noexcept).
    pageManagers[buffer.allocInfo.page].release(buffer.allocInfo);
    device.destroyBuffer(buffer.vkBuffer);
}


inline void Memory::configureBuffer(vk::Buffer& vkBuffer, const MemoryAllocationInfo& allocInfo,
                                    const uint32_t pageIndex) {

    try {

    	const auto& memoryPage = memoryPages[pageIndex];
        device.bindBufferMemory(vkBuffer, memoryPage, allocInfo.offset);

    } catch (...) {

        device.destroyBuffer(vkBuffer);
        throw;  // rethrow
    }
}


inline std::shared_ptr<ll::Buffer> Memory::buildBuffer(const vk::Buffer vkBuffer,
    const ll::impl::MemoryAllocationTryInfo & tryInfo) {

    try {

        // ll::Buffer can throw exception.
        auto buffer = std::shared_ptr<ll::Buffer>{new ll::Buffer {vkBuffer, this, tryInfo.allocInfo}};
        pageManagers[tryInfo.allocInfo.page].commitAllocation(tryInfo);
        return buffer;

    } catch (...) {

        device.destroyBuffer(vkBuffer);
        throw; // rethrow
    }
}


} // namespace ll

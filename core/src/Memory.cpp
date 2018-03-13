#include "lluvia/core/Memory.h"

#include "lluvia/core/Buffer.h"
#include "lluvia/core/Image.h"
#include "lluvia/core/ImageDescriptor.h"
#include "lluvia/core/MemoryAllocationInfo.h"

#include <algorithm>
#include <iostream>


namespace ll {

constexpr const vk::ImageLayout InitialImageLayout = vk::ImageLayout::eUndefined;


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


vk::MemoryPropertyFlags Memory::getMemoryPropertyFlags() const noexcept {
    return heapInfo.flags;
}


uint64_t Memory::getPageSize() const noexcept {
    return pageSize;
}


uint32_t Memory::getPageCount() const noexcept {
    return static_cast<uint32_t>(memoryPages.size());
}


bool Memory::isMappable() const noexcept {
    return (heapInfo.flags & vk::MemoryPropertyFlagBits::eHostVisible) == vk::MemoryPropertyFlagBits::eHostVisible;
}


std::shared_ptr<ll::Buffer> Memory::createBuffer(const uint64_t size, const vk::BufferUsageFlags usageFlags) {

    vk::BufferCreateInfo bufferInfo = vk::BufferCreateInfo()
                                      .setSharingMode(vk::SharingMode::eExclusive)
                                      .setSize(size)
                                      .setUsage(usageFlags)
                                      .setQueueFamilyIndexCount(heapInfo.familyQueueIndices.size())
                                      .setPQueueFamilyIndices(heapInfo.familyQueueIndices.data());

    // It's safe to not guard this call with a try-catch. If an
    // exception is thrown, let the caller to handle it. The memory
    // manager is still in its original state.
    auto vkBuffer = device.createBuffer(bufferInfo);

    // query alignment and offset
    const auto memRequirements = device.getBufferMemoryRequirements(vkBuffer);

    // TODO: check that memRequirements.memoryTypeBits is supported in this memory

    // find or create a new memory page where the buffer can be allocated
    auto tryInfo = getSuitableMemoryPage(memRequirements);
    
    

    // build a ll::Buffer object and commit the allocation if the
    // object construction is successful.
    try {
        
        const auto& memoryPage = memoryPages[tryInfo.allocInfo.page];
        device.bindBufferMemory(vkBuffer, memoryPage, tryInfo.allocInfo.offset);

        // ll::Buffer can throw exception.
        auto buffer = std::shared_ptr<ll::Buffer>{new ll::Buffer {vkBuffer, usageFlags, shared_from_this(), tryInfo.allocInfo, size}};
        pageManagers[tryInfo.allocInfo.page].commitAllocation(tryInfo);
        return buffer;

    } catch (...) {

        device.destroyBuffer(vkBuffer);
        throw; // rethrow
    }
}


void Memory::releaseBuffer(const ll::Buffer& buffer) {

    releaseMemoryAllocation(buffer.allocInfo);
    device.destroyBuffer(buffer.vkBuffer);
}


void* Memory::mapBuffer(const ll::Buffer& buffer) {

    const auto page   = buffer.allocInfo.page;
    const auto offset = buffer.allocInfo.offset + buffer.allocInfo.leftPadding;
    const auto size   = buffer.allocInfo.size;

    return device.mapMemory(memoryPages[page], offset, size);
}


void Memory::unmapBuffer(const ll::Buffer& buffer) {

    const auto page = buffer.allocInfo.page;
    device.unmapMemory(memoryPages[page]);
}


std::shared_ptr<ll::Image> Memory::createImage(const ll::ImageDescriptor& descriptor, const vk::ImageUsageFlags usageFlags) {

    // TODO: check that image witdh, height and depth are within supported range of the device

    auto imgInfo = vk::ImageCreateInfo {}
                    .setExtent({descriptor.getWidth(), descriptor.getHeight(), descriptor.getDepth()})
                    .setImageType(descriptor.getImageType())
                    .setArrayLayers(1)
                    .setMipLevels(1)
                    .setTiling(vk::ImageTiling::eOptimal)
                    .setSamples(vk::SampleCountFlagBits::e1)
                    .setSharingMode(vk::SharingMode::eExclusive)
                    .setUsage(usageFlags)
                    .setFormat(descriptor.getFormat())
                    .setInitialLayout(InitialImageLayout);

    auto vkImage = device.createImage(imgInfo);

    // query alignment and offset
    const auto memRequirements = device.getImageMemoryRequirements(vkImage);

    // TODO: check that memRequirements.memoryTypeBits is supported in this memory

    // find or create a new memory page where the image can be allocated
    auto tryInfo = getSuitableMemoryPage(memRequirements);
    
    try {
        const auto& memoryPage = memoryPages[tryInfo.allocInfo.page];
        device.bindImageMemory(vkImage, memoryPage, tryInfo.allocInfo.offset);

        auto image = std::shared_ptr<ll::Image> {new ll::Image {device,
                                                                vkImage,
                                                                descriptor,
                                                                shared_from_this(),
                                                                tryInfo.allocInfo,
                                                                InitialImageLayout,
                                                                usageFlags}};
                                                                
        pageManagers[tryInfo.allocInfo.page].commitAllocation(tryInfo);
        return image;

    } catch (...) {

        device.destroyImage(vkImage);
        throw;  // rethrow
    }
}


void Memory::releaseImage(const ll::Image& image) {

    releaseMemoryAllocation(image.allocInfo);
    device.destroyImage(image.vkImage);
}


void Memory::accept(ll::Visitor* visitor) {
    assert(visitor != nullptr);
    // nothing to visit
}


impl::MemoryAllocationTryInfo Memory::getSuitableMemoryPage(const vk::MemoryRequirements& memRequirements) {

    auto tryInfo = impl::MemoryAllocationTryInfo {};
    auto pageIndex = 0u;
    for (auto& manager : pageManagers) {

        if (manager.tryAllocate(memRequirements.size, memRequirements.alignment, tryInfo)) {
            tryInfo.allocInfo.page = pageIndex;
            manager.reserveManagerSpace();
            return tryInfo;
        }

        ++ pageIndex;
    }


    // None of the existing pages could allocate the object. Create a ne
    // memory page and allocate the object in it.

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
                                          .setMemoryTypeIndex(heapInfo.typeIndex);


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
    tryInfo.allocInfo.page = pageIndex;

    return tryInfo;
}


void Memory::releaseMemoryAllocation(const ll::MemoryAllocationInfo& allocInfo) {

    // reserve space in case the release of this allocation requires
    // the insertion of a new free interval.
    pageManagers[allocInfo.page].reserveManagerSpace();

    // release the allocated memory regardless of the result of
    // reserveManagerSpace(). In case it failed the release method
    // should throw and exception and abort the program (since it is
    // declared as noexcept).
    pageManagers[allocInfo.page].release(allocInfo);
}


} // namespace ll

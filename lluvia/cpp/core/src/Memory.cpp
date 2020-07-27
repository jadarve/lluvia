/**
@file       Memory.cpp
@brief      Memory class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#include "lluvia/core/Memory.h"

#include "lluvia/core/Buffer.h"
#include "lluvia/core/error.h"
#include "lluvia/core/Image.h"
#include "lluvia/core/ImageDescriptor.h"
#include "lluvia/core/ImageView.h"
#include "lluvia/core/ImageViewDescriptor.h"
#include "lluvia/core/MemoryAllocationInfo.h"
#include "lluvia/core/Session.h"

#include "lluvia/core/vulkan/Device.h"

#include <algorithm>
#include <exception>
#include <iostream>


constexpr const uint32_t CAPACITY_INCREASE = 32;

namespace ll {

constexpr const vk::ImageLayout InitialImageLayout = vk::ImageLayout::eUndefined;


Memory::Memory(
    const std::shared_ptr<ll::vulkan::Device>& device,
    const ll::VkHeapInfo& heapInfo,
    const uint64_t pageSize):
    m_device              {device},
    m_heapInfo            (heapInfo),
    m_pageSize            {pageSize} {
    
}


Memory::~Memory() {

    for (auto& memory : m_memoryPages) {
        m_device->get().freeMemory(memory);
    }
}


vk::MemoryPropertyFlags Memory::getMemoryPropertyFlags() const noexcept {
    return m_heapInfo.flags;
}


uint64_t Memory::getPageSize() const noexcept {
    return m_pageSize;
}


uint32_t Memory::getPageCount() const noexcept {
    return static_cast<uint32_t>(m_memoryPages.size());
}


bool Memory::isMappable() const noexcept {
    return (m_heapInfo.flags & vk::MemoryPropertyFlagBits::eHostVisible) == vk::MemoryPropertyFlagBits::eHostVisible;
}


bool Memory::isPageMappable(const uint32_t page) const noexcept {

    if (page < m_memoryPageMappingFlags.size()) {
        return isMappable() && !m_memoryPageMappingFlags[page];
    }
    
    return false;
}


std::shared_ptr<ll::Buffer> Memory::createBuffer(const uint64_t size) {

    const auto usageFlags = vk::BufferUsageFlags { vk::BufferUsageFlagBits::eStorageBuffer
                                                 | vk::BufferUsageFlagBits::eTransferSrc
                                                 | vk::BufferUsageFlagBits::eTransferDst};

    return createBuffer(size, usageFlags);
}

std::shared_ptr<ll::Buffer> Memory::createBufferWithUnsafeFlags(const uint64_t size, const uint32_t usageFlags) {

    // no check of the actual bits of usageFlags is performed
    return createBuffer(size,
        static_cast<vk::BufferUsageFlags>(usageFlags));
}


std::shared_ptr<ll::Buffer> Memory::createBuffer(const uint64_t size, const vk::BufferUsageFlags usageFlags) {

    vk::BufferCreateInfo bufferInfo = vk::BufferCreateInfo()
                                      .setSharingMode(vk::SharingMode::eExclusive)
                                      .setSize(size)
                                      .setUsage(usageFlags)
                                      .setQueueFamilyIndexCount(static_cast<uint32_t>(m_heapInfo.familyQueueIndices.size()))
                                      .setPQueueFamilyIndices(m_heapInfo.familyQueueIndices.data());

    // It's safe to not guard this call with a try-catch. If an
    // exception is thrown, let the caller to handle it. The memory
    // manager is still in its original state.
    auto vkBuffer = m_device->get().createBuffer(bufferInfo);

    // query alignment and offset
    const auto memRequirements = m_device->get().getBufferMemoryRequirements(vkBuffer);

    // check that memRequirements.memoryTypeBits is supported in this memory
    const auto memoryTypeBits = static_cast<uint32_t>(0x01 << m_heapInfo.typeIndex);
    if ((memoryTypeBits & memRequirements.memoryTypeBits) == 0u) {
        throw std::system_error(createErrorCode(ll::ErrorCode::ObjectAllocationError), "memory " + std::to_string(m_heapInfo.typeIndex) + " does not support allocating buffer objects.");
    }

    // find or create a new memory page where the buffer can be allocated
    auto tryInfo = getSuitableMemoryPage(memRequirements);
    
    

    // build a ll::Buffer object and commit the allocation if the
    // object construction is successful.
    try {
        
        const auto& memoryPage = m_memoryPages[tryInfo.allocInfo.page];
        m_device->get().bindBufferMemory(vkBuffer, memoryPage, tryInfo.allocInfo.offset);

        // ll::Buffer can throw exception.
        auto buffer = std::shared_ptr<ll::Buffer>{new ll::Buffer {vkBuffer, usageFlags, shared_from_this(), tryInfo.allocInfo, size}};
        m_pageManagers[tryInfo.allocInfo.page].commitAllocation(tryInfo);
        return buffer;

    } catch (...) {

        m_device->get().destroyBuffer(vkBuffer);
        throw; // rethrow
    }
}


void Memory::releaseBuffer(const ll::Buffer& buffer) {

    releaseMemoryAllocation(buffer.m_allocInfo);
    m_device->get().destroyBuffer(buffer.m_vkBuffer);
}


void* Memory::mapBuffer(const ll::Buffer& buffer) {

    const auto page   = buffer.m_allocInfo.page;
    const auto offset = buffer.m_allocInfo.offset + buffer.m_allocInfo.leftPadding;
    const auto size   = buffer.m_allocInfo.size;

    if (m_memoryPageMappingFlags[page]) {
        throw std::system_error {ll::createErrorCode(ll::ErrorCode::MemoryMapFailed), "Memory page [" + std::to_string(page) + "] is already mapped by another object."};
    }

    // set mapping flag for this page to mapped
    m_memoryPageMappingFlags[page] = true;
    return m_device->get().mapMemory(m_memoryPages[page], offset, size);
}


void Memory::unmapBuffer(const ll::Buffer& buffer) {

    const auto page = buffer.m_allocInfo.page;

    if (!m_memoryPageMappingFlags[page]) {
        throw std::system_error {ll::createErrorCode(ll::ErrorCode::MemoryMapFailed), "Memory page [" + std::to_string(page) + "] has not been mapped by any object."};
    }

    m_device->get().unmapMemory(m_memoryPages[page]);

    // set mapping flag for this page to unmapped
    m_memoryPageMappingFlags[page] = false;
}


std::shared_ptr<ll::Image> Memory::createImage(const ll::ImageDescriptor& descriptor) {

    ll::throwSystemErrorIf(descriptor.getWidth() == 0, ll::ErrorCode::InvalidArgument, "Image width must be greater than zero, got: " + std::to_string(descriptor.getWidth()));
    ll::throwSystemErrorIf(descriptor.getHeight() == 0, ll::ErrorCode::InvalidArgument, "Image height must be greater than zero, got: " + std::to_string(descriptor.getHeight()));
    ll::throwSystemErrorIf(descriptor.getDepth() == 0, ll::ErrorCode::InvalidArgument, "Image depth must be greater than zero, got: " + std::to_string(descriptor.getDepth()));

    // checks if the combination of image shape, tiling and flags can be used.
    ll::throwSystemErrorIf(!m_device->isImageDescriptorSupported(descriptor),
        ll::ErrorCode::ObjectAllocationError,
        "physical device does not support allocation of image objects with the provided "
        "combination of shape, tiling and usageFlags.");

    auto imgInfo = vk::ImageCreateInfo {}
                    .setExtent({descriptor.getWidth(), descriptor.getHeight(), descriptor.getDepth()})
                    .setImageType(descriptor.getImageType())
                    .setArrayLayers(1)
                    .setMipLevels(1)
                    .setTiling(descriptor.getTiling())
                    .setSamples(vk::SampleCountFlagBits::e1)
                    .setSharingMode(vk::SharingMode::eExclusive)
                    .setUsage(descriptor.getUsageFlags())
                    .setFormat(descriptor.getFormat())
                    .setInitialLayout(InitialImageLayout);

    auto vkImage = m_device->get().createImage(imgInfo);

    // query alignment and offset
    const auto memRequirements = m_device->get().getImageMemoryRequirements(vkImage);

    // check that memRequirements.memoryTypeBits is supported in this memory
    const auto memoryTypeBits = static_cast<uint32_t>(0x01 << m_heapInfo.typeIndex);
    if ((memoryTypeBits & memRequirements.memoryTypeBits) == 0u) {

        m_device->get().destroyImage(vkImage);
        throw std::system_error(createErrorCode(ll::ErrorCode::ObjectAllocationError), "memory " + std::to_string(m_heapInfo.typeIndex) + " does not support allocating image objects.");
    }

    // find or create a new memory page where the image can be allocated
    auto tryInfo = getSuitableMemoryPage(memRequirements);
    
    try {
        const auto& memoryPage = m_memoryPages[tryInfo.allocInfo.page];
        m_device->get().bindImageMemory(vkImage, memoryPage, tryInfo.allocInfo.offset);

        auto image = std::shared_ptr<ll::Image>{new ll::Image{m_device,
                                                              vkImage,
                                                              descriptor,
                                                              shared_from_this(),
                                                              tryInfo.allocInfo,
                                                              InitialImageLayout}};

        m_pageManagers[tryInfo.allocInfo.page].commitAllocation(tryInfo);
        return image;

    } catch (...) {

        m_device->get().destroyImage(vkImage);
        throw;  // rethrow
    }
}


std::shared_ptr<ll::ImageView> Memory::createImageView(
        const ll::ImageDescriptor& imgDescriptor,
        const ll::ImageViewDescriptor& viewDescriptor) {

    auto image = this->createImage(imgDescriptor);
    return image->createImageView(viewDescriptor);
}


void Memory::releaseImage(const ll::Image& image) {

    releaseMemoryAllocation(image.getAllocationInfo());
    m_device->get().destroyImage(image.m_vkImage);
}


impl::MemoryAllocationTryInfo Memory::getSuitableMemoryPage(const vk::MemoryRequirements& memRequirements) {

    auto tryInfo = impl::MemoryAllocationTryInfo {};
    auto pageIndex = 0u;
    for (auto& manager : m_pageManagers) {

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

    const auto newPageSize = std::max(m_pageSize, memRequirements.size);


    // reserve space to store a new memory page and manager.
    if (m_memoryPages.size() == m_memoryPages.capacity()) {
        m_memoryPages.reserve(m_memoryPages.capacity() + CAPACITY_INCREASE);
    }

    if (m_pageManagers.size() == m_pageManagers.capacity()) {
        m_pageManagers.reserve(m_pageManagers.capacity() + CAPACITY_INCREASE);
    }

    if (m_memoryPageMappingFlags.size() == m_memoryPageMappingFlags.capacity()) {
        m_memoryPageMappingFlags.reserve(m_memoryPageMappingFlags.capacity() + CAPACITY_INCREASE);
    }


    vk::MemoryAllocateInfo allocateInfo = vk::MemoryAllocateInfo()
                                          .setAllocationSize(newPageSize)
                                          .setMemoryTypeIndex(m_heapInfo.typeIndex);


    // Safe to not try-catch the creation of manager and memory.
    // If exception is thrown, this object is left in its previous
    // state plus the reserved space in memoryPages and pageManagers.
    auto manager = impl::MemoryFreeSpaceManager {newPageSize};
    manager.reserveManagerSpace();

    auto memory = m_device->get().allocateMemory(allocateInfo);

    // push objects to vectors after reserving space
    m_memoryPages.push_back(memory);
    m_pageManagers.push_back(std::move(manager));
    m_memoryPageMappingFlags.push_back(false);

    // this allocation try is guaranteed to work as there is enough
    // free space in the page to fit memRequirements.size.
    m_pageManagers[pageIndex].tryAllocate(memRequirements.size, memRequirements.alignment, tryInfo);
    tryInfo.allocInfo.page = pageIndex;

    return tryInfo;
}


void Memory::releaseMemoryAllocation(const ll::MemoryAllocationInfo& allocInfo) {

    // reserve space in case the release of this allocation requires
    // the insertion of a new free interval.
    m_pageManagers[allocInfo.page].reserveManagerSpace();

    // release the allocated memory regardless of the result of
    // reserveManagerSpace(). In case it failed the release method
    // should throw and exception and abort the program (since it is
    // declared as noexcept).
    m_pageManagers[allocInfo.page].release(allocInfo);
}


} // namespace ll

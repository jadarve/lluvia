#include "lluvia/core/Session.h"

#include <algorithm>
#include <exception>
#include <iostream>

namespace ll {

using namespace std;

Session::Session() :
    handle {std::make_shared<impl::SessionHandle>()} {

    init();
}


Session::~Session() {

    if(handle.use_count() == 1) {

        // destroy all buffers
        // destroy memory managers

        handle->device.destroy();
        handle->instance.destroy();
    }
}


std::vector<vk::MemoryPropertyFlags> Session::getSupportedMemoryFlags() const {

    auto memProperties = handle->physicalDevice.getMemoryProperties();

    auto memoryFlags = std::vector<vk::MemoryPropertyFlags> {};
    memoryFlags.reserve(memProperties.memoryTypeCount);

    for(auto i = 0u; i < memProperties.memoryTypeCount; ++ i) {

        auto flags = memProperties.memoryTypes[i].propertyFlags;

        // filter out flags with all bits set to 0
        if(flags == vk::MemoryPropertyFlags()) continue;

        // insert flags if it is not present in memoryFlags
        if(std::find(memoryFlags.begin(), memoryFlags.end(), flags) == memoryFlags.end()) {
            memoryFlags.push_back(flags);
        }
    }

    return memoryFlags;
}


std::tuple<bool, uint32_t> Session::configureMemory(const vk::MemoryPropertyFlags flags, const uint64_t pageSize) {

    // can throw?
    auto memProperties = handle->physicalDevice.getMemoryProperties();

    for(auto i = 0u; i < memProperties.memoryTypeCount; ++ i) {

        auto memType = memProperties.memoryTypes[i];
        if(memType.propertyFlags == flags) {
            
            auto heapInfo = ll::VkHeapInfo {};
            heapInfo.heapIndex = memType.heapIndex;
            heapInfo.size = memProperties.memoryHeaps[0].size;

            // TODO
            // heapInfo.familyQueueIndices = 

            // can throw exception. Invariants of Session are kept.
            memories.push_back(ll::Memory {handle->device, heapInfo, pageSize});

            return std::make_tuple(true, memories.size() - 1);
        }
    }

    return std::make_tuple(false, 0);
}


void Session::init() {

    auto instanceCreated = false;
    auto deviceCreated = false;

    try {

        instanceCreated = initInstance();
        deviceCreated = initDevice();
        initQueue();

    } catch(...) {

        if(deviceCreated) {
            handle->device.destroy();
        }

        if(instanceCreated) {
            handle->instance.destroy();
        }

        // rethrow
        throw;
    }
    
}


bool Session::initInstance() {
    
    auto appInfo = vk::ApplicationInfo()
        .setPApplicationName("lluvia")
        .setApplicationVersion(0)
        .setEngineVersion(0)
        .setPEngineName("lluvia")
        .setApiVersion(VK_MAKE_VERSION(1, 0, 54));

    auto instanceInfo = vk::InstanceCreateInfo()
        .setPApplicationInfo(&appInfo);

    auto result = vk::createInstance(&instanceInfo, nullptr, &handle->instance);

    if(result == vk::Result::eErrorIncompatibleDriver) {
        throw std::system_error(std::error_code(), "Incompatible driver");
    }

    // TODO: let user to choose physical device
    handle->physicalDevice = handle->instance.enumeratePhysicalDevices()[0];

    return true;
}


bool Session::initDevice() {
    
    auto queuePriority = 1.0f;
    handle->computeQueueFamilyIndex = getComputeFamilyQueueIndex();

    auto devQueueCreateInfo = vk::DeviceQueueCreateInfo()
        .setQueueCount(1)
        .setQueueFamilyIndex(handle->computeQueueFamilyIndex)
        .setPQueuePriorities(&queuePriority);

    auto devCreateInfo = vk::DeviceCreateInfo()
        .setQueueCreateInfoCount(1)
        .setPQueueCreateInfos(&devQueueCreateInfo);

    handle->device = handle->physicalDevice.createDevice(devCreateInfo);
    return true;
}


bool Session::initQueue() {
    
    // get the first compute capable queue
    handle->queue = handle->device.getQueue(handle->computeQueueFamilyIndex, 0);
    return true;
}


uint32_t Session::getComputeFamilyQueueIndex() {
    
    auto queueProperties = handle->physicalDevice.getQueueFamilyProperties();

    uint32_t queueIndex = 0;
    for(auto prop : queueProperties) {
        
        auto compute = ((prop.queueFlags & vk::QueueFlagBits::eCompute) == vk::QueueFlagBits::eCompute);
        if(compute != 0) {
            return queueIndex;
        }

        ++queueIndex;
    }

    throw std::system_error(std::error_code(), "No compute capable queue family found.");
}

} // namespace ll
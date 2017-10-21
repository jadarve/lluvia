#include "lluvia/core/Session.h"

#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>

namespace ll {

using namespace std;

Session::Session():
    referenceCounter    {std::make_shared<int>(0)} {

    auto instanceCreated = false;
    auto deviceCreated = false;

    try {

        instanceCreated = initInstance();
        deviceCreated = initDevice();
        initQueue();

    } catch (...) {

        if (deviceCreated) {
            device.destroy();
        }

        if (instanceCreated) {
            instance.destroy();
        }

        // rethrow
        throw;
    }
}


Session::~Session() {

    if (referenceCounter.use_count() == 1) {

        // destroy all buffers
        // destroy memory managers

        device.destroy();
        instance.destroy();
    }
}


std::vector<vk::MemoryPropertyFlags> Session::getSupportedMemoryFlags() const {

    const auto memProperties = physicalDevice.getMemoryProperties();

    auto memoryFlags = std::vector<vk::MemoryPropertyFlags> {};
    memoryFlags.reserve(memProperties.memoryTypeCount);

    for (auto i = 0u; i < memProperties.memoryTypeCount; ++ i) {

        const auto flags = memProperties.memoryTypes[i].propertyFlags;

        // filter out flags with all bits set to 0
        if (flags == vk::MemoryPropertyFlags()) continue;

        // insert flags if it is not present in memoryFlags
        if (std::find(memoryFlags.begin(), memoryFlags.end(), flags) == memoryFlags.end()) {
            memoryFlags.push_back(flags);
        }
    }

    return memoryFlags;
}


std::tuple<bool, uint32_t> Session::configureMemory(const vk::MemoryPropertyFlags flags, const uint64_t pageSize) {

    // can throw?
    const auto memProperties = physicalDevice.getMemoryProperties();

    for (auto i = 0u; i < memProperties.memoryTypeCount; ++ i) {

        const auto memType = memProperties.memoryTypes[i];
        if (memType.propertyFlags == flags) {

            auto heapInfo = ll::VkHeapInfo {};
            heapInfo.heapIndex = memType.heapIndex;
            heapInfo.size = memProperties.memoryHeaps[0].size;

            // TODO
            // heapInfo.familyQueueIndices =

            // can throw exception. Invariants of Session are kept.
            memories.push_back(ll::Memory {device, heapInfo, pageSize});

            return std::make_tuple(true, memories.size() - 1);
        }
    }

    return std::make_tuple(false, 0);
}


std::tuple<bool, ll::Shader> Session::createShader(const std::string& spirvPath) const {

    auto file = ifstream {spirvPath, std::ios::ate | std::ios::binary};

    if (file.is_open()) {

        auto fileSize = (size_t) file.tellg();
        auto spirvCode = std::vector<char> {};
        spirvCode.reserve(fileSize);

        file.seekg(0);
        file.read(spirvCode.data(), fileSize);
        file.close();

        return std::make_tuple(true, Shader {device, spirvCode});
    }

    return std::make_tuple(false, Shader {});
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

    const auto result = vk::createInstance(&instanceInfo, nullptr, &instance);

    if (result == vk::Result::eErrorIncompatibleDriver) {
        throw std::system_error(std::error_code(), "Incompatible driver");
    }

    // TODO: let user to choose physical device
    physicalDevice = instance.enumeratePhysicalDevices()[0];

    return true;
}


bool Session::initDevice() {

    const auto queuePriority = 1.0f;
    computeQueueFamilyIndex = getComputeFamilyQueueIndex();

    auto devQueueCreateInfo = vk::DeviceQueueCreateInfo()
                              .setQueueCount(1)
                              .setQueueFamilyIndex(computeQueueFamilyIndex)
                              .setPQueuePriorities(&queuePriority);

    auto devCreateInfo = vk::DeviceCreateInfo()
                         .setQueueCreateInfoCount(1)
                         .setPQueueCreateInfos(&devQueueCreateInfo);

    device = physicalDevice.createDevice(devCreateInfo);
    return true;
}


bool Session::initQueue() {

    // get the first compute capable queue
    queue = device.getQueue(computeQueueFamilyIndex, 0);
    return true;
}


uint32_t Session::getComputeFamilyQueueIndex() {

    const auto queueProperties = physicalDevice.getQueueFamilyProperties();

    uint32_t queueIndex = 0;
    for (auto prop : queueProperties) {

        const auto compute = ((prop.queueFlags & vk::QueueFlagBits::eCompute) == vk::QueueFlagBits::eCompute);
        if (compute != 0) {
            return queueIndex;
        }

        ++ queueIndex;
    }

    throw std::system_error(std::error_code(), "No compute capable queue family found.");
}

} // namespace ll
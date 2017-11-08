#include "lluvia/core/Session.h"

#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>

namespace ll {

using namespace std;

std::unique_ptr<Session> Session::create() {

    auto session = std::unique_ptr<Session>{new Session()};
    return session;
}

Session::Session() {

    auto instanceCreated = false;
    auto deviceCreated   = false;

    try {

        instanceCreated = initInstance();
        deviceCreated   = initDevice();
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

    device.destroy();
    instance.destroy();
}


std::vector<vk::MemoryPropertyFlags> Session::getSupportedMemoryFlags() const {

    const auto memProperties = physicalDevice.getMemoryProperties();
          auto memoryFlags   = std::vector<vk::MemoryPropertyFlags> {};

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


std::unique_ptr<ll::Memory> Session::createMemory(const vk::MemoryPropertyFlags flags, const uint64_t pageSize) const {

    const auto memProperties = physicalDevice.getMemoryProperties();

    for (auto i = 0u; i < memProperties.memoryTypeCount; ++ i) {

        const auto& memType = memProperties.memoryTypes[i];
        if (memType.propertyFlags == flags) {

            auto heapInfo = ll::VkHeapInfo {};

            heapInfo.heapIndex          = memType.heapIndex;
            heapInfo.size               = memProperties.memoryHeaps[memType.heapIndex].size;
            heapInfo.familyQueueIndices = std::vector<uint32_t> {computeQueueFamilyIndex};

            // can throw exception. Invariants of Session are kept.
            return std::make_unique<ll::Memory>(device, heapInfo, pageSize);
        }
    }

    return nullptr;
}


std::shared_ptr<const ll::Program> Session::createProgram(const std::string& spirvPath) const {

    // workaround for GCC 4.8
    ifstream file {spirvPath, std::ios::ate | std::ios::binary};

    if (file.is_open()) {

        const auto fileSize  = static_cast<size_t>(file.tellg());
              auto spirvCode = std::vector<char>(fileSize);

        file.seekg(0);
        file.read(spirvCode.data(), fileSize);
        file.close();

        return std::make_shared<const ll::Program>(device, spirvCode);
    }

    return nullptr;
}


std::unique_ptr<ll::ComputeNode> Session::createComputeNode(const ll::ComputeNodeDescriptor& descriptor) {

    return std::make_unique<ll::ComputeNode>(device, descriptor);
}


void Session::run(const std::shared_ptr<ll::ComputeNode> node) {

    vk::CommandPoolCreateInfo createInfo = vk::CommandPoolCreateInfo()
        .setQueueFamilyIndex(computeQueueFamilyIndex);

    vk::CommandPool commandPool = device.createCommandPool(createInfo);

    vk::CommandBufferAllocateInfo allocInfo = vk::CommandBufferAllocateInfo()
        .setCommandPool(commandPool)
        .setCommandBufferCount(1);

    vector<vk::CommandBuffer> cmdBuffers = device.allocateCommandBuffers(allocInfo);
    vk::CommandBuffer cmdBuffer = cmdBuffers[0];

    // record command buffer
    vk::CommandBufferBeginInfo beginInfo = vk::CommandBufferBeginInfo()
        .setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    cmdBuffer.begin(beginInfo);
    node->record(cmdBuffer);
    cmdBuffer.end();

    // Here I can set semaphores to wait for and 
    // semaphores to trigger once execution is completed.
    vk::SubmitInfo submitInfo = vk::SubmitInfo()
        .setCommandBufferCount(1)
        .setPCommandBuffers(&cmdBuffer);

    queue.submit(1, &submitInfo, nullptr);
    queue.waitIdle();

    device.destroyCommandPool(commandPool, nullptr);
}


bool Session::initInstance() {

    auto appInfo = vk::ApplicationInfo()
                   .setPApplicationName("lluvia")
                   .setApplicationVersion(0)
                   .setEngineVersion(0)
                   .setPEngineName("lluvia")
                   .setApiVersion(VK_MAKE_VERSION(1, 0, 65));

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

    auto queueIndex = uint32_t {0};
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

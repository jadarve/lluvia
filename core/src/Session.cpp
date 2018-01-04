#include "lluvia/core/Session.h"

#include "lluvia/core/Buffer.h"
#include "lluvia/core/CommandBuffer.h"
#include "lluvia/core/ComputeNode.h"
#include "lluvia/core/ComputeNodeDescriptor.h"
#include "lluvia/core/Memory.h"
#include "lluvia/core/Program.h"

#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>

namespace ll {

using namespace std;

std::unique_ptr<ll::Session> Session::create() {

    return std::unique_ptr<Session>{new Session()};
}


std::vector<vk::LayerProperties> Session::getVulkanInstanceLayerProperties() {
    
    return vk::enumerateInstanceLayerProperties();
}


std::vector<vk::ExtensionProperties> Session::getVulkanExtensionProperties() {
    
    return vk::enumerateInstanceExtensionProperties();
}


Session::Session() {

    auto instanceCreated = false;
    auto deviceCreated   = false;

    try {

        instanceCreated = initInstance();
        deviceCreated   = initDevice();
        initQueue();
        initCommandPool();

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

    device.destroyCommandPool(commandPool);
    device.destroy();
    instance.destroy();
}


vk::PhysicalDeviceMemoryProperties Session::getPhysicalDeviceMemoryProperties() const {

    return physicalDevice.getMemoryProperties();
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


std::unique_ptr<ll::Memory> Session::createMemory(const vk::MemoryPropertyFlags flags, const uint64_t pageSize, bool exactFlagsMatch) const {
    
    auto compareFlags = [](const auto& flags, const auto& value, bool exactFlagsMatch) {
        return exactFlagsMatch? flags == value : (flags & value) == value;
    };
    
    const auto memProperties = physicalDevice.getMemoryProperties();

    for (auto i = 0u; i < memProperties.memoryTypeCount; ++ i) {

        const auto& memType = memProperties.memoryTypes[i];
        
        if (compareFlags(memType.propertyFlags, flags, exactFlagsMatch)) {

            auto heapInfo = ll::VkHeapInfo {};

            heapInfo.typeIndex          = i;
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


std::shared_ptr<ll::ComputeNode> Session::createComputeNode(const ll::ComputeNodeDescriptor& descriptor) const {

    return std::make_shared<ll::ComputeNode>(device, descriptor);
}


std::shared_ptr<ll::CommandBuffer> Session::createCommandBuffer() const {

    return std::make_shared<ll::CommandBuffer>(device, commandPool);
}

void Session::run(const std::shared_ptr<ll::ComputeNode> node) {

    auto commandPool = vk::CommandPool {};
    auto commandBuffer = vk::CommandBuffer {};
    std::tie(commandPool, commandBuffer) = createOneTimeSubmitCommandBuffer();

    // record command buffer
    vk::CommandBufferBeginInfo beginInfo = vk::CommandBufferBeginInfo()
        .setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    commandBuffer.begin(beginInfo);
    node->record(commandBuffer);
    commandBuffer.end();

    // Here I can set semaphores to wait for and 
    // semaphores to trigger once execution is completed.
    vk::SubmitInfo submitInfo = vk::SubmitInfo()
        .setCommandBufferCount(1)
        .setPCommandBuffers(&commandBuffer);

    queue.submit(1, &submitInfo, nullptr);
    queue.waitIdle();

    device.destroyCommandPool(commandPool, nullptr);
}


void Session::copyBuffer(const ll::Buffer& src, const ll::Buffer& dst) {

    assert(dst.getSize() >= src.getSize());

    auto commandPool = vk::CommandPool {};
    auto commandBuffer = vk::CommandBuffer {};
    std::tie(commandPool, commandBuffer) = createOneTimeSubmitCommandBuffer();

    // record command buffer
    vk::CommandBufferBeginInfo beginInfo = vk::CommandBufferBeginInfo()
        .setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    auto copyInfo = vk::BufferCopy()
        .setSrcOffset(0)
        .setDstOffset(0)
        .setSize(src.getSize());

    commandBuffer.begin(beginInfo);
    commandBuffer.copyBuffer(src.vkBuffer, dst.vkBuffer, 1, &copyInfo);
    commandBuffer.end();

    // Here I can set semaphores to wait for and 
    // semaphores to trigger once execution is completed.
    vk::SubmitInfo submitInfo = vk::SubmitInfo()
        .setCommandBufferCount(1)
        .setPCommandBuffers(&commandBuffer);

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


bool Session::initCommandPool() {

    const auto createInfo = vk::CommandPoolCreateInfo()
                                .setQueueFamilyIndex(computeQueueFamilyIndex);

    commandPool = device.createCommandPool(createInfo);
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


std::tuple<vk::CommandPool, vk::CommandBuffer> Session::createOneTimeSubmitCommandBuffer() {

    vk::CommandPoolCreateInfo createInfo = vk::CommandPoolCreateInfo()
        .setQueueFamilyIndex(computeQueueFamilyIndex);

    vk::CommandPool commandPool = device.createCommandPool(createInfo);

    vk::CommandBufferAllocateInfo allocInfo = vk::CommandBufferAllocateInfo()
        .setCommandPool(commandPool)
        .setCommandBufferCount(1);

    vector<vk::CommandBuffer> cmdBuffers = device.allocateCommandBuffers(allocInfo);
    vk::CommandBuffer cmdBuffer = cmdBuffers[0];

    return std::make_tuple(commandPool, cmdBuffer);
}

} // namespace ll

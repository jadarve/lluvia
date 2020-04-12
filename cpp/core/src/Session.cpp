/**
@file       Session.cpp
@brief      Session class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#include "lluvia/core/Session.h"

#include "lluvia/core/Buffer.h"
#include "lluvia/core/CommandBuffer.h"
#include "lluvia/core/ComputeNode.h"
#include "lluvia/core/ComputeNodeDescriptor.h"
#include "lluvia/core/ContainerNode.h"
#include "lluvia/core/ContainerNodeDescriptor.h"
#include "lluvia/core/Duration.h"
#include "lluvia/core/error.h"
#include "lluvia/core/Image.h"
#include "lluvia/core/ImageDescriptor.h"
#include "lluvia/core/Interpreter.h"
#include "lluvia/core/Memory.h"
#include "lluvia/core/Program.h"

#include "lluvia/core/vulkan/CommandPool.h"
#include "lluvia/core/vulkan/Device.h"
#include "lluvia/core/vulkan/Instance.h"

#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>

namespace ll {

using namespace std;

std::shared_ptr<ll::Session> Session::create() {
    return std::shared_ptr<Session>{new Session()};;
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

    // FIXME: once all the VK objects are managed, this try-catch can disappear :)
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
            m_instance.reset();
        }

        // rethrow
        throw;
    }

    m_interpreter = std::make_unique<ll::Interpreter>();

    // by sending a raw pointer, I avoid a circular reference
    // of shared pointers between the interpreter and this session.
    m_interpreter->setActiveSession(this);

    initHostMemory();
}


Session::~Session() {

    m_hostMemory.reset();
    m_programRegistry.clear();

    device.destroyCommandPool(commandPool);
    device.destroy();
}


std::shared_ptr<ll::Memory> Session::getHostMemory() const noexcept {
    return m_hostMemory;
}


const std::unique_ptr<ll::Interpreter>& Session::getInterpreter() const noexcept {
    return m_interpreter;
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


bool Session::isImageDescriptorSupported(const ll::ImageDescriptor& descriptor) const noexcept {

    auto formatProperties = vk::ImageFormatProperties {};

    auto result = physicalDevice.getImageFormatProperties(descriptor.getFormat(),
        descriptor.getImageType(), 
        descriptor.getTiling(), 
        descriptor.getUsageFlags(),
        vk::ImageCreateFlags {},
        &formatProperties);

    if (result != vk::Result::eSuccess) {
        return false;
    }

    // check extend
    if (descriptor.getWidth() > formatProperties.maxExtent.width ||
        descriptor.getHeight() > formatProperties.maxExtent.height ||
        descriptor.getDepth() > formatProperties.maxExtent.depth) {
        return false;
    }

    return true;
}


std::shared_ptr<ll::Memory> Session::createMemory(const vk::MemoryPropertyFlags flags, const uint64_t pageSize, bool exactFlagsMatch) {
    
    // memory objects returned externally are set to keep a reference to this session.
    return createMemoryImpl(flags, pageSize, exactFlagsMatch, true);
}


std::shared_ptr<ll::Program> Session::createProgram(const std::string& spirvPath) const {

    auto spirvCode = std::vector<uint8_t>();

    try {
        // workaround for GCC 4.8
        ifstream file{spirvPath, std::ios::ate | std::ios::binary};
        file.exceptions(std::ifstream::badbit | std::ifstream::failbit);

        const auto fileSize = static_cast<size_t>(file.tellg());
        spirvCode.resize(fileSize);

        file.seekg(0);
        file.read(reinterpret_cast<char *>(spirvCode.data()), fileSize);
        file.close();

    } catch (std::ios_base::failure& fail) {
        ll::throwSystemError(ll::ErrorCode::IOError,
            "Error reading SPIR-V file '" + spirvPath + "', error: " + fail.what());
    }

    return createProgram(spirvCode);
}


std::shared_ptr<ll::Program> Session::createProgram(const std::vector<uint8_t>& spirv) const {
    
    return std::make_shared<ll::Program>(shared_from_this(), device, spirv);
}


void Session::setProgram(const std::string& name, const std::shared_ptr<ll::Program>& program) {

    ll::throwSystemErrorIf(program == nullptr, ll::ErrorCode::InvalidArgument, "program parameter must be not null");

    m_programRegistry.insert_or_assign(name, program);
}


std::shared_ptr<ll::Program> Session::getProgram(const std::string& name) const {

    auto iter = m_programRegistry.find(name);

    if (iter == m_programRegistry.cend()) {
        // FIXME: what to do?
        return nullptr;
    }

    return iter->second;
}


std::shared_ptr<ll::ComputeNode> Session::createComputeNode(const ll::ComputeNodeDescriptor& descriptor) {

    return std::shared_ptr<ll::ComputeNode> {new ll::ComputeNode {shared_from_this(), device, descriptor}};
}


std::shared_ptr<ll::ComputeNode> Session::createComputeNode(const std::string& builderName) {

    // create a ComputeNodeDescriptor using the builder
    // call Session::createComputeNode using that descriptor
    // TOTHINK: How to handle Lua errors?
    
    const auto descriptor = createComputeNodeDescriptor(builderName);
    return createComputeNode(descriptor);
}


ll::ComputeNodeDescriptor Session::createComputeNodeDescriptor(const std::string& builderName) const {

    // FIXME: need to distinguish between Compute and Container builders
    constexpr auto lua = R"(
        local builderName = ...
        local builder = ll.getNodeBuilder(builderName)
        return builder.newDescriptor()
    )";

    return m_interpreter->loadAndRun<ll::ComputeNodeDescriptor>(lua, builderName);
}


std::shared_ptr<ll::ContainerNode> Session::createContainerNode(const ll::ContainerNodeDescriptor& descriptor) {

    return std::shared_ptr<ll::ContainerNode> {new ll::ContainerNode {shared_from_this(), descriptor}};
}


std::shared_ptr<ll::ContainerNode> Session::createContainerNode(const std::string& builderName) {

    const auto descriptor = createContainerNodeDescriptor(builderName);
    return createContainerNode(descriptor);
}


ll::ContainerNodeDescriptor Session::createContainerNodeDescriptor(const std::string& builderName) const {

    // FIXME: need to distinguish between Compute and Container builders
    constexpr auto lua = R"(
        local builderName = ...
        local builder = ll.getNodeBuilder(builderName)
        return builder.newDescriptor()
    )";

    return m_interpreter->loadAndRun<ll::ContainerNodeDescriptor>(lua, builderName);
}

std::unique_ptr<ll::Duration> Session::createDuration() const {

    return std::make_unique<ll::Duration>(device);
}


std::unique_ptr<ll::CommandBuffer> Session::createCommandBuffer() const {

    return std::make_unique<ll::CommandBuffer>(device, commandPool);
}


void Session::run(const ll::CommandBuffer& cmdBuffer) {

    vk::SubmitInfo submitInfo = vk::SubmitInfo()
        .setCommandBufferCount(1)
        .setPCommandBuffers(&cmdBuffer.m_commandBuffer);

    queue.submit(1, &submitInfo, nullptr);
    queue.waitIdle();
}


void Session::run(const ll::ComputeNode& node) {

    auto cmdBuffer = createCommandBuffer();

    cmdBuffer->begin();
    cmdBuffer->run(node);
    cmdBuffer->end();

    run(*cmdBuffer);
}


void Session::run(const ll::ContainerNode& node) {

    auto cmdBuffer = createCommandBuffer();

    cmdBuffer->begin();
    node.record(*cmdBuffer);
    cmdBuffer->end();

    run(*cmdBuffer);
}


void Session::script(const std::string &code) {
    m_interpreter->run(code);
}


void Session::scriptFile(const std::string& filename) {
    m_interpreter->runFile(filename);
}


bool Session::initInstance() {
    
    auto instance = vk::Instance {};

    auto appInfo = vk::ApplicationInfo()
                   .setPApplicationName("lluvia")
                   .setApplicationVersion(0)
                   .setEngineVersion(0)
                   .setPEngineName("lluvia")
                   .setApiVersion(VK_MAKE_VERSION(1, 0, 65));

    auto instanceInfo = vk::InstanceCreateInfo()
                        .setPApplicationInfo(&appInfo);

    const auto result = vk::createInstance(&instanceInfo, nullptr, &instance);
    ll::throwSystemErrorIf(result == vk::Result::eErrorIncompatibleDriver,
                           ll::ErrorCode::InconpatibleDriver, "Inconpatible driver.");

    m_instance = std::make_shared<ll::vulkan::Instance>(instance);

    const auto physicalDevices = m_instance->get().enumeratePhysicalDevices();
    ll::throwSystemErrorIf(physicalDevices.size() == 0,
        ll::ErrorCode::PhysicalDevicesNotFound, "No physical devices found in the system");

    // TODO: let user to choose physical device
    physicalDevice = m_instance->get().enumeratePhysicalDevices()[0];

    return true;
}


bool Session::initDevice() {

    const auto queuePriority = 1.0f;

    computeQueueFamilyIndex = getComputeFamilyQueueIndex();

    auto devQueueCreateInfo = vk::DeviceQueueCreateInfo()
                              .setQueueCount(1)
                              .setQueueFamilyIndex(computeQueueFamilyIndex)
                              .setPQueuePriorities(&queuePriority);

    const auto supportedFeatures = physicalDevice.getFeatures();

    auto desiredFeatures = vk::PhysicalDeviceFeatures {}
        .setShaderStorageImageExtendedFormats(supportedFeatures.shaderStorageImageExtendedFormats);

    auto devCreateInfo = vk::DeviceCreateInfo()
                         .setQueueCreateInfoCount(1)
                         .setPQueueCreateInfos(&devQueueCreateInfo)
                         .setPEnabledFeatures(&desiredFeatures);

    device = physicalDevice.createDevice(devCreateInfo);

    m_device = std::make_shared<ll::vulkan::Device>(device, m_instance);
    return true;
}


bool Session::initQueue() {

    // get the first compute capable queue
    queue = m_device->get().getQueue(computeQueueFamilyIndex, 0);
    return true;
}


bool Session::initCommandPool() {

    const auto createInfo = vk::CommandPoolCreateInfo()
                                .setQueueFamilyIndex(computeQueueFamilyIndex);

    commandPool = m_device->get().createCommandPool(createInfo);

    m_commandPool = std::make_shared<ll::vulkan::CommandPool>(commandPool, m_device);
    return true;
}

void Session::initHostMemory() {

    // as m_hostMemory is a member of this class, there is no need to keep a reference
    // to this pointer.
    m_hostMemory = createMemoryImpl(
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        0, false, false);
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

std::shared_ptr<ll::Memory> Session::createMemoryImpl(const vk::MemoryPropertyFlags flags,
                                                      const uint64_t pageSize,
                                                      bool exactFlagsMatch,
                                                      bool keepThisSharedReference) {

    auto compareFlags = [](const auto &tFlags, const auto &value, bool tExactFlagsMatch) {
        return tExactFlagsMatch ? tFlags == value : (tFlags & value) == value;
    };

    const auto memProperties = physicalDevice.getMemoryProperties();

    for (auto i = 0u; i < memProperties.memoryTypeCount; ++i)
    {

        const auto &memType = memProperties.memoryTypes[i];

        if (compareFlags(memType.propertyFlags, flags, exactFlagsMatch))
        {

            auto heapInfo = ll::VkHeapInfo{};

            heapInfo.typeIndex = i;
            heapInfo.size = memProperties.memoryHeaps[memType.heapIndex].size;
            heapInfo.flags = memType.propertyFlags;
            heapInfo.familyQueueIndices = std::vector<uint32_t>{computeQueueFamilyIndex};

            // can throw exception. Invariants of Session are kept.
            return std::make_shared<ll::Memory>(keepThisSharedReference? shared_from_this() : nullptr,
                                                device, heapInfo, pageSize);
        }
    }

    throw std::system_error(createErrorCode(ll::ErrorCode::MemoryCreationError),
                            "No memory was found that matched the requested flags.");
}

} // namespace ll

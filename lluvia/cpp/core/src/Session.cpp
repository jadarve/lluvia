/**
@file       Session.cpp
@brief      Session class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

// define this macro to use Vulkan's dynamic dispatcher by default
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include "lluvia/core/Session.h"

#include "lluvia/core/buffer/Buffer.h"
#include "lluvia/core/CommandBuffer.h"
#include "lluvia/core/node/ComputeNode.h"
#include "lluvia/core/node/ComputeNodeDescriptor.h"
#include "lluvia/core/node/ContainerNodeDescriptor.h"
#include "lluvia/core/node/ContainerNode.h"
#include "lluvia/core/Duration.h"
#include "lluvia/core/error.h"
#include "lluvia/core/image/Image.h"
#include "lluvia/core/image/ImageDescriptor.h"
#include "lluvia/core/Interpreter.h"
#include "lluvia/core/memory/Memory.h"
#include "lluvia/core/Program.h"

#include "lluvia/core/impl/ZipArchive.h"

#include "lluvia/core/vulkan/Device.h"
#include "lluvia/core/vulkan/Instance.h"

#include <cstring>
#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>

namespace ll {

using namespace std;

std::shared_ptr<ll::Session> Session::create() {
    return create(ll::SessionDescriptor {});
}

std::shared_ptr<ll::Session> Session::create(const ll::SessionDescriptor& descriptor) {
    return std::shared_ptr<Session>{new Session(descriptor)};
}

std::vector<ll::DeviceDescriptor> Session::getAvailableDevices() {

    auto devices = std::vector<ll::DeviceDescriptor>{};

    auto vulkanInstance = std::make_shared<ll::vulkan::Instance>(false);
    const auto vulkanPhysicalDevices = vulkanInstance->get().enumeratePhysicalDevices();

    for (const auto vkPhysicalDevice : vulkanPhysicalDevices) {

        const auto vkDeviceProperties = vkPhysicalDevice.getProperties();

        auto desc = DeviceDescriptor{
            vkDeviceProperties.deviceID,
            ll::impl::fromVkPhysicalDeviceType(vkDeviceProperties.deviceType),
            vkDeviceProperties.deviceName
            };

        devices.push_back(desc);
    }

    return devices;
}

std::vector<vk::LayerProperties> Session::getVulkanInstanceLayerProperties() {
    
    return vk::enumerateInstanceLayerProperties();
}

std::vector<vk::ExtensionProperties> Session::getVulkanExtensionProperties() {
    
    return vk::enumerateInstanceExtensionProperties();
}


Session::Session(const ll::SessionDescriptor& descriptor):
    m_descriptor {descriptor} {

    m_instance = std::make_shared<ll::vulkan::Instance>(m_descriptor.isDebugEnabled());

    initDescriptor();
    initDevice();

    // by sending a raw pointer, I avoid a circular reference
    // of shared pointers between the interpreter and this session.
    m_interpreter = std::make_shared<ll::Interpreter>();
    m_interpreter->setActiveSession(this);

    m_hostMemory = createMemory(
        ll::MemoryPropertyFlagBits::HostVisible | ll::MemoryPropertyFlagBits::HostCoherent,
        0, false);
}


Session::~Session() {
    // there might be nodes out there that still want to access the
    // session through their Lua build scripts. By setting the active
    // session to null, I should be safe of memory leaks.
    m_interpreter->setActiveSession(nullptr);
}


std::shared_ptr<ll::Memory> Session::getHostMemory() const noexcept {
    return m_hostMemory;
}


vk::PhysicalDeviceMemoryProperties Session::getPhysicalDeviceMemoryProperties() const {

    return m_device->getPhysicalDevice().getMemoryProperties();
}


std::vector<ll::MemoryPropertyFlags> Session::getSupportedMemoryFlags() const {

    const auto memProperties = m_device->getPhysicalDevice().getMemoryProperties();
          auto memoryFlags   = std::vector<ll::MemoryPropertyFlags> {};

    memoryFlags.reserve(memProperties.memoryTypeCount);

    for (auto i = 0u; i < memProperties.memoryTypeCount; ++ i) {

        const auto flags = ll::impl::fromVkMemoryPropertyFlags(memProperties.memoryTypes[i].propertyFlags);

        // filter out flags with all bits set to 0
        if (flags == ll::MemoryPropertyFlags{}) continue;

        // insert flags if it is not present in memoryFlags
        if (std::find(memoryFlags.begin(), memoryFlags.end(), flags) == memoryFlags.end()) {
            memoryFlags.push_back(flags);
        }
    }

    return memoryFlags;
}


const ll::DeviceDescriptor& Session::getDeviceDescriptor() const noexcept {
    return m_deviceDescriptor;
}

bool Session::isImageDescriptorSupported(const ll::ImageDescriptor& descriptor) const noexcept {

    return m_device->isImageDescriptorSupported(descriptor);
}


std::shared_ptr<ll::Memory> Session::createMemory(const ll::MemoryPropertyFlags& flags, const uint64_t pageSize, bool exactFlagsMatch) {
    
    auto compareFlags = [](const auto &tFlags, const auto &value, bool tExactFlagsMatch) {
        return tExactFlagsMatch ? tFlags == value : (tFlags & value) == value;
    };

    const auto memProperties = m_device->getPhysicalDevice().getMemoryProperties();

    for (auto i = 0u; i < memProperties.memoryTypeCount; ++i) {

        const auto &memType = memProperties.memoryTypes[i];

        const auto memoryPropertyFlags = ll::impl::fromVkMemoryPropertyFlags(memType.propertyFlags);

        if (compareFlags(memoryPropertyFlags, flags, exactFlagsMatch)) {

            auto heapInfo = ll::VkHeapInfo{};

            heapInfo.typeIndex = i;
            heapInfo.size = memProperties.memoryHeaps[memType.heapIndex].size;
            heapInfo.flags = memoryPropertyFlags;
            heapInfo.familyQueueIndices = std::vector<uint32_t>{m_device->getComputeFamilyQueueIndex()};

            // can throw exception. Invariants of Session are kept.
            return std::make_shared<ll::Memory>(m_device, heapInfo, pageSize);
        }
    }

    throw std::system_error(createErrorCode(ll::ErrorCode::MemoryCreationError),
                            "No memory was found that matched the requested flags.");
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
    
    return std::make_shared<ll::Program>(m_device, spirv);
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


std::vector<ll::NodeBuilderDescriptor> Session::getNodeBuilderDescriptors() const {

    constexpr auto lua = R"(
        local fromCpp = ...
        local descriptors = ll.getNodeBuilderDescriptors()

        for _, v in ipairs(descriptors) do
            table.insert(fromCpp, v)
        end
    )";

    auto output = std::vector<ll::NodeBuilderDescriptor>{};
    m_interpreter->loadAndRunNoReturn(lua, output);
    
    return output;
}


std::shared_ptr<ll::ComputeNode> Session::createComputeNode(const ll::ComputeNodeDescriptor& descriptor) {

    return std::make_shared<ll::ComputeNode>(m_device, descriptor, m_interpreter);
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

    return std::make_shared<ll::ContainerNode>(m_interpreter, descriptor);
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

    return std::make_unique<ll::Duration>(m_device);
}


std::unique_ptr<ll::CommandBuffer> Session::createCommandBuffer() const {

    return m_device->createCommandBuffer();
}


void Session::run(const ll::CommandBuffer& cmdBuffer) {

    m_device->run(cmdBuffer);
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

void Session::loadLibrary(const std::string& filename) {

    constexpr const auto LUA_EXTENSION = ".lua";
    constexpr const auto SPV_EXTENSION = ".spv";
    constexpr const auto EXTENSION_LENGTH = 4;

    auto archive = ll::impl::ZipArchive {filename};
    const auto numberFiles = archive.numberFiles();
    for (auto i = 0u; i < numberFiles; ++i) {

        auto stat = archive.getFileStat(i);
        auto filepath = std::string{stat.m_filename};

        // ignore filepaths whose length is less than the extension length
        if (filepath.size() < EXTENSION_LENGTH) {
            continue;
        }

        if (filepath.compare(filepath.size() - EXTENSION_LENGTH, EXTENSION_LENGTH, LUA_EXTENSION) == 0) {

            auto luaScript = archive.uncompressTextFile(stat);
            script(luaScript);
        }
        else if (filepath.compare(filepath.size() - EXTENSION_LENGTH, EXTENSION_LENGTH, SPV_EXTENSION) == 0) {
            auto spirv = archive.uncompressBinaryFile(stat);

            auto program = createProgram(spirv);
            auto programName = filepath.substr(0, filepath.size() - EXTENSION_LENGTH);
            setProgram(programName, program);
        }
    }
}

ll::vec3ui Session::getGoodComputeLocalShape(ll::ComputeDimension dimensions) const noexcept {

    // FIXME: add device-specific logic
    switch(dimensions) {
        case ll::ComputeDimension::D1:
            return ll::vec3ui{1024, 1, 1};
        case ll::ComputeDimension::D2:
            return ll::vec3ui{32, 32, 1};
        case ll::ComputeDimension::D3:
            return ll::vec3ui{16, 16, 4};
    }
}

std::string Session::help(const std::string& builderName) const {

    constexpr auto lua = R"(
        local builderName = ...
        local builder = ll.getNodeBuilder(builderName)
        return builder.doc
    )";

    return m_interpreter->loadAndRun<std::string>(lua, builderName);
}

bool Session::hasReceivedVulkanWarningMessages() const noexcept {

    return m_instance->hasReceivedVulkanWarningMessages();
}

void Session::initDescriptor() {

    // assign the device descriptor if it is present in the session descriptor
    if (m_descriptor.getDeviceDescriptor().has_value()) {
        m_deviceDescriptor = m_descriptor.getDeviceDescriptor().value();
    } else {
        // otherwise, select the first available device
        const auto physicalDevices = m_instance->get().enumeratePhysicalDevices();
        ll::throwSystemErrorIf(physicalDevices.empty(),
                               ll::ErrorCode::PhysicalDevicesNotFound, "No physical devices found in the system");
        
        auto physicalDevice = physicalDevices[0];

        m_deviceDescriptor = DeviceDescriptor{
            physicalDevice.getProperties().deviceID,
            ll::impl::fromVkPhysicalDeviceType(physicalDevice.getProperties().deviceType),
            physicalDevice.getProperties().deviceName};
    }
}

void Session::initDevice() {

    const auto physicalDevices = m_instance->get().enumeratePhysicalDevices();
    ll::throwSystemErrorIf(physicalDevices.empty(),
        ll::ErrorCode::PhysicalDevicesNotFound, "No physical devices found in the system");

    auto findPhysicalDeviceCondition = [this](const vk::PhysicalDevice& physicalDevice) {
        return physicalDevice.getProperties().deviceID == m_deviceDescriptor.id;
    };
    
    auto it = std::find_if(physicalDevices.cbegin(), physicalDevices.cend(), findPhysicalDeviceCondition);
    ll::throwSystemErrorIf(it == physicalDevices.cend(),
        ll::ErrorCode::PhysicalDevicesNotFound, "Unable to find physical device with id: " + std::to_string(m_deviceDescriptor.id));

    auto physicalDevice = *it;

    const auto queuePriority = 1.0f;

    auto computeQueueFamilyIndex = findComputeFamilyQueueIndex(physicalDevice);

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

    auto device = physicalDevice.createDevice(devCreateInfo);
    VULKAN_HPP_DEFAULT_DISPATCHER.init(device);

    m_device = std::make_shared<ll::vulkan::Device>(device, physicalDevice, computeQueueFamilyIndex, m_instance);
}

uint32_t Session::findComputeFamilyQueueIndex(vk::PhysicalDevice& physicalDevice) {

    const auto queueProperties = physicalDevice.getQueueFamilyProperties();

    auto queueIndex = uint32_t {0};
    for (const auto& prop : queueProperties) {

        const auto compute = ((prop.queueFlags & vk::QueueFlagBits::eCompute) == vk::QueueFlagBits::eCompute);

        if (compute != 0) {
            return queueIndex;
        }

        ++ queueIndex;
    }

    ll::throwSystemError(ll::ErrorCode::PhysicalDevicesNotFound, "No compute capable queue family found.");
    return 0;
}

} // namespace ll

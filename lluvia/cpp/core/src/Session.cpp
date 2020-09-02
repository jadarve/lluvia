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

#include "lluvia/core/impl/ZipArchive.h"

#include "lluvia/core/vulkan/Device.h"
#include "lluvia/core/vulkan/Instance.h"

#if defined(__GNUC__)
// Ensure we get the 64-bit variants of the CRT's file I/O calls
#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE 1
#endif
#endif

#include "miniz.h"

#include <cstring>
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

    initDevice();

    // by sending a raw pointer, I avoid a circular reference
    // of shared pointers between the interpreter and this session.
    m_interpreter = std::make_shared<ll::Interpreter>();
    m_interpreter->setActiveSession(this);

    m_hostMemory = createMemory(
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
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


std::vector<vk::MemoryPropertyFlags> Session::getSupportedMemoryFlags() const {

    const auto memProperties = m_device->getPhysicalDevice().getMemoryProperties();
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

    return m_device->isImageDescriptorSupported(descriptor);
}


std::shared_ptr<ll::Memory> Session::createMemory(const vk::MemoryPropertyFlags flags, const uint64_t pageSize, bool exactFlagsMatch) {
    
    auto compareFlags = [](const auto &tFlags, const auto &value, bool tExactFlagsMatch) {
        return tExactFlagsMatch ? tFlags == value : (tFlags & value) == value;
    };

    const auto memProperties = m_device->getPhysicalDevice().getMemoryProperties();

    for (auto i = 0u; i < memProperties.memoryTypeCount; ++i) {

        const auto &memType = memProperties.memoryTypes[i];

        if (compareFlags(memType.propertyFlags, flags, exactFlagsMatch)) {

            auto heapInfo = ll::VkHeapInfo{};

            heapInfo.typeIndex = i;
            heapInfo.size = memProperties.memoryHeaps[memType.heapIndex].size;
            heapInfo.flags = memType.propertyFlags;
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


std::shared_ptr<ll::ComputeNode> Session::createComputeNode(const ll::ComputeNodeDescriptor& descriptor) {

    return std::shared_ptr<ll::ComputeNode> {new ll::ComputeNode {m_device, descriptor, m_interpreter}};
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

    return std::shared_ptr<ll::ContainerNode> {new ll::ContainerNode {m_interpreter, descriptor}};
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


void Session::initDevice() {

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
    auto physicalDevice = m_instance->get().enumeratePhysicalDevices()[0];

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

    m_device = std::make_shared<ll::vulkan::Device>(device, physicalDevice, computeQueueFamilyIndex, m_instance);
}

uint32_t Session::findComputeFamilyQueueIndex(vk::PhysicalDevice& physicalDevice) {

    const auto queueProperties = physicalDevice.getQueueFamilyProperties();

    auto queueIndex = uint32_t {0};
    for (auto prop : queueProperties) {

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

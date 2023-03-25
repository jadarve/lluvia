/**
 * \file test_SessionCreation.cpp
 * \brief test Session creation
 * \copyright 2017, Juan David Adarve. See AUTHORS for more details
 * \license Apache 2.0, see LICENSE for more details
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include <iostream>
#include <vector>

#include "lluvia/core.h"

bool containMemoryFlags(const ll::MemoryPropertyFlags& actual, const ll::MemoryPropertyFlags& contains)
{
    return (actual & contains) == contains;
}

TEST_CASE("GetAvaialableDevices", "SessionCreationTest")
{

    const auto availableDevices = ll::Session::getAvailableDevices();

    for (const auto& desc : availableDevices) {
        auto deviceType = desc.deviceType;

        std::cout << "ID: " << desc.id
                  << " name: " << desc.name
                  << " type: " << ll::deviceTypeToString(std::forward<ll::DeviceType>(deviceType)) << std::endl;
    }

    REQUIRE(availableDevices.size() != 0);
}

TEST_CASE("DefaultParameters", "SessionCreationTest")
{

    auto session = ll::Session::create();
}

TEST_CASE("DebugEnabled", "SessionCreationTest")
{

    auto desc = ll::SessionDescriptor().enableDebug(true);

    auto session = std::shared_ptr<ll::Session> {nullptr};
    REQUIRE_NOTHROW(session = ll::Session::create(desc));
    REQUIRE(session != nullptr);

    REQUIRE_FALSE(session->hasReceivedVulkanWarningMessages());
}

TEST_CASE("MultipleDevicesAvailable", "SessionCreationTest")
{

    const auto availableDevices = ll::Session::getAvailableDevices();

    for (auto deviceDesc : availableDevices) {

        std::cout << "ID: " << deviceDesc.id
                  << " type: " << ll::deviceTypeToString(std::forward<ll::DeviceType>(deviceDesc.deviceType))
                  << " name: " << deviceDesc.name << std::endl;

        auto desc = ll::SessionDescriptor().enableDebug(true).setDeviceDescriptor(deviceDesc);

        auto session = ll::Session::create(desc);

        auto sessionDeviceDesc = session->getDeviceDescriptor();

        REQUIRE(deviceDesc == sessionDeviceDesc);
        REQUIRE_FALSE(session->hasReceivedVulkanWarningMessages());
    }
}

// TEST_CASE("TriggerDebugError", "SessionCreationTest")
// {

//     const auto availableDevices = ll::Session::getAvailableDevices();

//     for (auto deviceDesc : availableDevices) {

//         std::cout << "ID: " << deviceDesc.id
//                   << " type: " << ll::deviceTypeToString(std::forward<ll::DeviceType>(deviceDesc.deviceType))
//                   << " name: " << deviceDesc.name << std::endl;

//         auto desc = ll::SessionDescriptor().enableDebug(true).setDeviceDescriptor(deviceDesc);

//         auto session = ll::Session::create(desc);

//         auto sessionDeviceDesc = session->getDeviceDescriptor();

//         REQUIRE(deviceDesc == sessionDeviceDesc);
//         REQUIRE_FALSE(session->hasReceivedVulkanWarningMessages());
//     }
// }

/**
 * Test that the returned memory flags meet the Vulkan specification
 *
 * From the Vulkan specification:
 *
 *  There must be at least one memory type with both the
 *  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT and
 *  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT bits set in its propertyFlags.
 *  There must be at least one memory type with the
 *  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT bit set in its propertyFlags.
 *
 */
TEST_CASE("MemoryFlags", "SessionCreationTest")
{

    auto session = ll::Session::create(ll::SessionDescriptor().enableDebug(true));

    auto memoryFlags = session->getSupportedMemoryFlags();

    auto hostVisibleCoherentFlags = ll::MemoryPropertyFlagBits::HostVisible | ll::MemoryPropertyFlagBits::HostCoherent;
    auto deviceLocalFlags         = ll::MemoryPropertyFlagBits::DeviceLocal;

    auto hostFlagsFound   = false;
    auto deviceFlagsFound = false;

    for (auto flags : memoryFlags) {

        if (containMemoryFlags(flags, hostVisibleCoherentFlags)) {
            hostFlagsFound = true;
        }

        if (containMemoryFlags(flags, deviceLocalFlags)) {
            deviceFlagsFound = true;
        }
    }

    REQUIRE(hostFlagsFound);
    REQUIRE(deviceFlagsFound);

    REQUIRE_FALSE(session->hasReceivedVulkanWarningMessages());
}

TEST_CASE("DefaultMemories", "SessionCreationTest")
{

    const auto hostVisibleCoherentFlags = ll::MemoryPropertyFlagBits::HostVisible | ll::MemoryPropertyFlagBits::HostCoherent;
    const auto deviceLocalFlags         = ll::MemoryPropertyFlagBits::DeviceLocal;

    ///////////////////////////////////////////////////////
    // for each available device
    const auto availableDevices = ll::Session::getAvailableDevices();
    for (auto deviceDesc : availableDevices) {

        std::cout << "ID: " << deviceDesc.id
                  << " type: " << ll::deviceTypeToString(std::forward<ll::DeviceType>(deviceDesc.deviceType))
                  << " name: " << deviceDesc.name << std::endl;

        ///////////////////////////////////////////////////
        // given a session with debug enabled
        auto desc = ll::SessionDescriptor().enableDebug(true).setDeviceDescriptor(deviceDesc);

        auto session = ll::Session::create(desc);

        ///////////////////////////////////////////////////
        // when the host default memory are requested
        auto hostMemory = session->getHostMemory();

        // then it should be not null
        REQUIRE(hostMemory != nullptr);

        // and should have the host visible and coherent flags
        REQUIRE(containMemoryFlags(hostMemory->getMemoryPropertyFlags(), hostVisibleCoherentFlags));

        ///////////////////////////////////////////////////
        // and when the device default memory are requested
        auto deviceMemory = session->getDeviceMemory();

        // then it should be not null
        REQUIRE(deviceMemory != nullptr);

        // and should have the device local flags
        REQUIRE(containMemoryFlags(deviceMemory->getMemoryPropertyFlags(), deviceLocalFlags));
    }
}

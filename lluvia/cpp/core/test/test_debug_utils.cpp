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

TEST_CASE("hasReceivedVulkanWarningMessages", "SessionCreationTest")
{

    const auto availableDevices = ll::Session::getAvailableDevices();

    for (auto deviceDesc : availableDevices) {

        std::cout << "ID: " << deviceDesc.id
                  << " type: " << ll::deviceTypeToString(std::forward<ll::DeviceType>(deviceDesc.deviceType))
                  << " name: " << deviceDesc.name << std::endl;

        auto desc = ll::SessionDescriptor().enableDebug(true).setDeviceDescriptor(deviceDesc);
        auto session = ll::Session::create(desc);
        REQUIRE(session != nullptr);

        auto memory = session->createMemory(ll::MemoryPropertyFlagBits::DeviceLocal, 0);
        REQUIRE(memory != nullptr);

        // create a source buffer without the ll::BufferUsageFlagBits::TransferSrc flag bit
        auto sourceBuffer = memory->createBuffer(1024, ll::BufferUsageFlagBits::StorageBuffer | ll::BufferUsageFlagBits::TransferDst);
        REQUIRE(sourceBuffer != nullptr);

        // create a destination buffer without the ll::BufferUsageFlagBits::TransferDst flag bit
        auto destinationBuffer = memory->createBuffer(1024, ll::BufferUsageFlagBits::StorageBuffer | ll::BufferUsageFlagBits::TransferSrc);
        REQUIRE(destinationBuffer != nullptr);

        // Executing the command buffer below should raise a warning message as the buffers are not
        // create with the correct set of usage flags.
        // Expected warnings:
        // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/html/vkspec.html#VUID-vkCmdCopyBuffer-srcBuffer-00118
        // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/html/vkspec.html#VUID-vkCmdCopyBuffer-dstBuffer-00120
        auto cmdBuffer = session->createCommandBuffer();
        REQUIRE(cmdBuffer != nullptr);
        cmdBuffer->begin();
        cmdBuffer->copyBuffer(*sourceBuffer, *destinationBuffer);
        cmdBuffer->end();

        session->run(*cmdBuffer);

        // use CHECK instead of REQUIRE as I want to run the test for all available devices.
        CHECK(session->hasReceivedVulkanWarningMessages());
    }
}

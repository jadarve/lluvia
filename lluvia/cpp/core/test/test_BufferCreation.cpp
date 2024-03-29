/**
 * \file test_BufferCreation.cpp
 * \brief test memory buffer creation
 * \copyright 2017, Juan David Adarve. See AUTHORS for more details
 * \license Apache 2.0, see LICENSE for more details
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "lluvia/core.h"
#include <iostream>

/**
 * Test that the returned memory flags meet the Vulkan speficiation
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
TEST_CASE("DeviceLocalBuffer", "test_BufferCreation")
{

    auto session = ll::Session::create(ll::SessionDescriptor().enableDebug(true));
    REQUIRE(session != nullptr);

    const auto memoryFlags = ll::MemoryPropertyFlagBits::DeviceLocal;

    auto memory = session->createMemory(memoryFlags, 2048, false);
    REQUIRE(memory != nullptr);

    auto buffer = memory->createBuffer(100);
    REQUIRE(buffer != nullptr);
    REQUIRE(buffer->getSize() == 100);
    REQUIRE_FALSE(session->hasReceivedVulkanWarningMessages());
}

TEST_CASE("HostVisibleCoherentCached", "test_BufferCreation")
{

    auto session = ll::Session::create(ll::SessionDescriptor().enableDebug(true));
    REQUIRE(session != nullptr);

    const auto memoryFlags = ll::MemoryPropertyFlagBits::HostVisible
                             | ll::MemoryPropertyFlagBits::HostCoherent;

    auto memory = session->createMemory(memoryFlags, 2048, false);
    REQUIRE(memory != nullptr);

    auto buffer = memory->createBuffer(100);
    REQUIRE(buffer != nullptr);
    REQUIRE(buffer->getSize() == 100);
    REQUIRE_FALSE(session->hasReceivedVulkanWarningMessages());
}

TEST_CASE("BufferSizeEqualPageSize", "test_BufferCreation")
{

    auto session = ll::Session::create(ll::SessionDescriptor().enableDebug(true));
    REQUIRE(session != nullptr);

    auto memory = session->createMemory(ll::MemoryPropertyFlagBits::DeviceLocal, 4096, false);
    REQUIRE(memory != nullptr);

    auto buffer0 = memory->createBuffer(128);
    REQUIRE(buffer0 != nullptr);
    REQUIRE(buffer0->getSize() == 128);
    auto buffer1 = memory->createBuffer(32);
    REQUIRE(buffer1 != nullptr);
    REQUIRE(buffer1->getSize() == 32);
    auto buffer2 = memory->createBuffer(768);
    REQUIRE(buffer2 != nullptr);
    REQUIRE(buffer2->getSize() == 768);
    auto buffer3 = memory->createBuffer(2048);
    REQUIRE(buffer3 != nullptr);
    REQUIRE(buffer3->getSize() == 2048);
    auto buffer4 = memory->createBuffer(4096);
    REQUIRE(buffer4 != nullptr);
    REQUIRE(buffer4->getSize() == 4096);

    REQUIRE_FALSE(session->hasReceivedVulkanWarningMessages());
}

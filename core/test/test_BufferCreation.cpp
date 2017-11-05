/**
 * \file test_BufferCreation.cpp
 * \brief test memory buffer creation
 * \copyright 2017, Juan David Adarve. See AUTHORS for more details
 * \license Apache 2.0, see LICENSE for more details
 */

#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"

#include <iostream>
#include <gtest/gtest.h>
#include "lluvia/core.h"


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
TEST_CASE("HostVisibleBuffer", "[BufferCreationTest]") {

    ll::Session session {};

    auto memoryFlags = session.getSupportedMemoryFlags();

    const auto hostVisibleCoherentFlags =   vk::MemoryPropertyFlagBits::eHostVisible
                                          | vk::MemoryPropertyFlagBits::eHostCoherent;
    // auto deviceLocalFlags = vk::MemoryPropertyFlagBits::eDeviceLocal;

    
    
    auto memory = session.createMemory(hostVisibleCoherentFlags, 1024);
    // ASSERT_EQ(memory.isValid(), true);

    auto buffer = memory.createBuffer(512);
    REQUIRE(buffer.isValid());

}

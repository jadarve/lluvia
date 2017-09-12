/**
 * \file test_SessionCreation.cpp
 * \brief test Session creation
 * \copyright 2017, Juan David Adarve. See AUTHORS for more details
 * \license Apache 2.0, see LICENSE for more details
 */

#include <iostream>
#include <gtest/gtest.h>
#include "lluvia/core.h"

TEST(SessionCreationTest, DefaultParameters) {

    ll::Session session {};
}


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
TEST(SessionCreationTest, MemoryFlags) {

    ll::Session session {};

    auto memoryFlags = session.getSupportedMemoryFlags();

    auto hostVisibleCoherentFlags = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
    auto deviceLocalFlags = vk::MemoryPropertyFlagBits::eDeviceLocal;

    auto hostFlagsFound = false;
    auto deviceFlagsFound = false;

    for(auto flags : memoryFlags) {

        if(flags == hostVisibleCoherentFlags) {
            hostFlagsFound = true;
        }

        if(flags == deviceLocalFlags) {
            deviceFlagsFound = true;
        }
    }

    ASSERT_EQ(true, hostFlagsFound && deviceFlagsFound);
}


int main(int argc, char **argv) {
    
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
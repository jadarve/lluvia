/**
 * \file test_SessionCreation.cpp
 * \brief test Session creation
 * \copyright 2017, Juan David Adarve. See AUTHORS for more details
 * \license Apache 2.0, see LICENSE for more details
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include <iostream>
#include "lluvia/core.h"

TEST_CASE("DefaultParameters", "[SessionCreationTest]") {

    auto session = ll::Session::create();
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
TEST_CASE("MemoryFlags", "[SessionCreationTest]") {
    
    auto containFlags = [](const auto& flags, const auto& value) {
        return (flags & value) == value;
    };

    auto session = ll::Session::create();

    auto memoryFlags = session->getSupportedMemoryFlags();

    auto hostVisibleCoherentFlags = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
    auto deviceLocalFlags = vk::MemoryPropertyFlagBits::eDeviceLocal;

    auto hostFlagsFound = false;
    auto deviceFlagsFound = false;

    for(auto flags : memoryFlags) {
        
        if (containFlags(flags, hostVisibleCoherentFlags)) {
            hostFlagsFound = true;
        }
        
        if (containFlags(flags, deviceLocalFlags)) {
            deviceFlagsFound = true;
        }
    }

    REQUIRE((hostFlagsFound && deviceFlagsFound) == true);
}

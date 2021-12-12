/**
 * \file test_ImageLayout.cpp
 * \brief test changing image layouts.
 * \copyright 2017, Juan David Adarve. See AUTHORS for more details
 * \license Apache 2.0, see LICENSE for more details
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include <iostream>
#include "lluvia/core.h"


TEST_CASE("UndefinedToGeneral", "test_ImageLayout") {

    auto session = ll::Session::create(ll::SessionDescriptor().enableDebug(true));
    REQUIRE(session != nullptr);

    const auto memoryFlags = ll::MemoryPropertyFlagBits::DeviceLocal;
    
    auto memory = session->createMemory(memoryFlags, 1024*1024*4, false);
    REQUIRE(memory != nullptr);

    const vk::ImageUsageFlags imgUsageFlags = { vk::ImageUsageFlagBits::eStorage
                                              | vk::ImageUsageFlagBits::eSampled
                                              | vk::ImageUsageFlagBits::eTransferDst};

    auto desc = ll::ImageDescriptor{}
                    .setWidth(640)
                    .setHeight(480)
                    .setChannelType(ll::ChannelType::Uint8)
                    .setChannelCount(ll::ChannelCount::C1)
                    .setUsageFlags(imgUsageFlags);

    auto image = memory->createImage(desc);
    REQUIRE(image != nullptr);

    // session->changeImageLayout(image, vk::ImageLayout::eGeneral);
    auto cmdBuffer = session->createCommandBuffer();
    REQUIRE(cmdBuffer != nullptr);

    cmdBuffer->begin();
    cmdBuffer->changeImageLayout(*image, vk::ImageLayout::eGeneral);
    cmdBuffer->end();
    
    std::cout << "running command buffer" << std::endl;
    session->run(*cmdBuffer);
    std::cout << "command buffer run successfully" << std::endl;

    REQUIRE_FALSE(ll::hasReceivedVulkanWarningMessages());
}


/**
 * \file test_ImageCreation.cpp
 * \brief test memory buffer creation
 * \copyright 2017, Juan David Adarve. See AUTHORS for more details
 * \license Apache 2.0, see LICENSE for more details
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include <iostream>
#include "lluvia/core.h"


TEST_CASE("DeviceLocalImage", "test_ImageCreation") {

    auto session = ll::Session::create(ll::SessionDescriptor().enableDebug(true));
    REQUIRE(session != nullptr);

    const auto memoryFlags = vk::MemoryPropertyFlagBits::eDeviceLocal;
    
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
    
    session->run(*cmdBuffer);

    auto imgViewDesc = ll::ImageViewDescriptor {}
                        .setFilterMode(ll::ImageFilterMode::Nearest)
                        .setAddressMode(ll::ImageAddressMode::ClampToEdge)
                        .setNormalizedCoordinates(false)
                        .setIsSampled(false);

    auto imageView = image->createImageView(imgViewDesc);
    REQUIRE(imageView != nullptr);

    REQUIRE_FALSE(ll::hasReceivedVulkanWarningMessages());
}


TEST_CASE("InvalidImageSize", "test_ImageCreation") {

    auto session = ll::Session::create(ll::SessionDescriptor().enableDebug(true));
    REQUIRE(session != nullptr);

    const auto memoryFlags = vk::MemoryPropertyFlagBits::eDeviceLocal;
    
    auto memory = session->createMemory(memoryFlags, 1024*1024*4, false);
    REQUIRE(memory != nullptr);

    const vk::ImageUsageFlags imgUsageFlags = { vk::ImageUsageFlagBits::eStorage
                                              | vk::ImageUsageFlagBits::eSampled
                                              | vk::ImageUsageFlagBits::eTransferDst};

    auto desc = ll::ImageDescriptor{}.setUsageFlags(imgUsageFlags);
    
    // create image from default constructed descriptor
    REQUIRE_NOTHROW(memory->createImage(desc));

    

    // valid dimensions
    desc.setWidth(32)
        .setHeight(32)
        .setDepth(32)
        .setChannelCount(ll::ChannelCount::C1);
    REQUIRE_NOTHROW(memory->createImage(desc));    

    // invalid width
    desc.setWidth(0)
        .setHeight(32)
        .setDepth(32)
        .setChannelCount(ll::ChannelCount::C1);
    REQUIRE_THROWS_AS(memory->createImage(desc), std::system_error);

    // invalid height
    desc.setWidth(32)
        .setHeight(0)
        .setDepth(32)
        .setChannelCount(ll::ChannelCount::C1);
    REQUIRE_THROWS_AS(memory->createImage(desc), std::system_error);

    // invalid depth
    desc.setWidth(32)
        .setHeight(32)
        .setDepth(0)
        .setChannelCount(ll::ChannelCount::C1);
    REQUIRE_THROWS_AS(memory->createImage(desc), std::system_error);

    REQUIRE_FALSE(ll::hasReceivedVulkanWarningMessages());
}

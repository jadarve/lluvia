/**
 * \file test_ImageCreation.cpp
 * \brief test memory buffer creation
 * \copyright 2017, Juan David Adarve. See AUTHORS for more details
 * \license Apache 2.0, see LICENSE for more details
 */

#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"

#include <iostream>
#include "lluvia/core.h"


TEST_CASE("DeviceLocalImage", "test_ImageCreation") {

    auto session = ll::Session::create();
    REQUIRE(session != nullptr);

    const auto memoryFlags = vk::MemoryPropertyFlagBits::eDeviceLocal;
    
    auto memory = session->createMemory(memoryFlags, 1024*1024*4, false);
    REQUIRE(memory != nullptr);

    auto desc = ll::ImageDescriptor{}
                    .setWidth(640)
                    .setHeight(480)
                    .setChannelType(ll::ChannelType::Uint8)
                    .setChannelCount(4);

    auto image = memory->createImage(desc);
    REQUIRE(image != nullptr);

    session->changeImageLayout(image, vk::ImageLayout::eGeneral);

    auto imgViewDesc = ll::ImageViewDescriptor {}
                        .setFilteringMode(ll::ImageFilterMode::Nearest)
                        .setAddressMode(ll::ImageAddressMode::ClampToEdge)
                        .setNormalizedCoordinates(false);

    auto imageView = image->createImageView(imgViewDesc);
    REQUIRE(imageView != nullptr);
}



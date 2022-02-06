/**
 * \file test_utils.cpp
 * \brief test utility functions
 * \copyright 2017, Juan David Adarve. See AUTHORS for more details
 * \license Apache 2.0, see LICENSE for more details
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "lluvia/core.h"

using memflags = ll::MemoryPropertyFlagBits;
#ifdef _WIN32
#define __attribute__()
#endif


TEST_CASE("createInitImage", "test_utils") {

    // Constants
    const auto memoryFlags = memflags::DeviceLocal;

    const auto width = uint32_t {1080};
    const auto height = uint32_t {1920};

    const ll::ImageUsageFlags imgUsageFlags = { ll::ImageUsageFlagBits::Storage
                                              | ll::ImageUsageFlagBits::Sampled
                                              | ll::ImageUsageFlagBits::TransferDst};

    const auto imgDesc = ll::ImageDescriptor {1, height, width, ll::ChannelCount::C1, ll::ChannelType::Uint8, imgUsageFlags, ll::ImageTiling::Optimal};


    auto session = ll::Session::create(ll::SessionDescriptor().enableDebug(true));
    auto memory = session->createMemory(memoryFlags, 0);

    // could create several images at the same time
    auto image = ll::createAndInitImage(session, memory, imgDesc, ll::ImageLayout::General);

    REQUIRE_FALSE(session->hasReceivedVulkanWarningMessages());
}


TEST_CASE("configureGraph", "test_utils") {

    // Constants
    const auto memoryFlags = memflags::DeviceLocal;

    const auto width = uint32_t {1080};
    const auto height = uint32_t {1920};

    const ll::ImageUsageFlags imgUsageFlags = { ll::ImageUsageFlagBits::Storage
                                              | ll::ImageUsageFlagBits::Sampled
                                              | ll::ImageUsageFlagBits::TransferDst};

    const auto imgDesc = ll::ImageDescriptor {1, height, width, ll::ChannelCount::C1, ll::ChannelType::Uint8, imgUsageFlags, ll::ImageTiling::Optimal};

    auto session = ll::Session::create(ll::SessionDescriptor().enableDebug(true));
    auto memory = session->createMemory(memoryFlags, 0);

    const auto RGBADesc = ll::ImageDescriptor(imgDesc).setChannelCount(ll::ChannelCount::C4);
    const auto grayDesc = ll::ImageDescriptor(imgDesc).setChannelCount(ll::ChannelCount::C1);

    // TOTHINK: Could initialiaze both images with a single command buffer. More efficient.
    auto RGBA = ll::createAndInitImage(session, memory, RGBADesc, ll::ImageLayout::General);
    auto gray = ll::createAndInitImage(session, memory, grayDesc, ll::ImageLayout::General);

    // auto rgba2GrayNode = session->readComputeNode("/home/jadarve/git/lluvia/local/nodes/RGBA2Gray.json");
    // configureComputeNode(rgba2GrayNode,
    //                      RGBA->getShape(),
    //                      {{0, RGBA}, {1, gray}} // grid
    //                     );

    // {RGBA->getWidth(), RGBA->getHeight(), RGBA->getDepth()}

    REQUIRE_FALSE(session->hasReceivedVulkanWarningMessages());
}

/**
 * \file test_ComputeNode.cpp
 * \brief test compute node.
 * \copyright 2017, Juan David Adarve. See AUTHORS for more details
 * \license Apache 2.0, see LICENSE for more details
 */

#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"

#include "coreTestConfig.h"

#include <cmath>
#include <iostream>
#include "lluvia/core.h"


TEST_CASE("BufferAssignment", "test_ComputeNodeImage") {

    constexpr const uint32_t WIDTH = 32u;
    constexpr const uint32_t HEIGHT = 32u;

    const vk::ImageUsageFlags imgUsageFlags = { vk::ImageUsageFlagBits::eStorage
                                              | vk::ImageUsageFlagBits::eSampled
                                              | vk::ImageUsageFlagBits::eTransferDst};

    using memflags = vk::MemoryPropertyFlagBits;

    auto session = ll::Session::create();
    REQUIRE(session != nullptr);

    const auto hostMemFlags = memflags::eHostVisible | memflags::eHostCoherent;
    const auto deviceMemFlags = memflags::eDeviceLocal;
    const auto pageSize = uint64_t {1024*1024*4};

    auto hostMemory = session->createMemory(hostMemFlags, pageSize, false);
    REQUIRE(hostMemory != nullptr);

    auto hostOutputMemory = session->createMemory(hostMemFlags, pageSize, false);
    REQUIRE(hostOutputMemory != nullptr);

    auto deviceMemory = session->createMemory(deviceMemFlags, pageSize, false);
    REQUIRE(deviceMemory != nullptr);

    // stage buffer (host visible)
    // device image
    // copy stage buffer to device image

    auto imgDesc = ll::ImageDescriptor {}
                    .setWidth(WIDTH)
                    .setHeight(HEIGHT)
                    .setChannelType(ll::ChannelType::Uint8)
                    .setChannelCount(1);

    auto stageBuffer  = hostMemory->createBuffer(imgDesc.getSize());
    auto outputBuffer = hostOutputMemory->createBuffer(imgDesc.getSize()*sizeof(uint32_t));

    auto bufMapped = static_cast<uint8_t*>(stageBuffer->map());

    // copy image data
    for (auto row = 0u; row < HEIGHT; ++ row) {
        for (auto col = 0u; col < WIDTH; ++ col) {
            bufMapped[row*HEIGHT + col] = std::min(1u, (col + (row % 2)) % 2);
        }
    }
    
    stageBuffer->unmap();

    auto image = deviceMemory->createImage(imgDesc, imgUsageFlags);

    auto imgViewDesc = ll::ImageViewDescriptor {}
                        .setAddressMode(ll::ImageAddressMode::Repeat)
                        .setFilteringMode(ll::ImageFilterMode::Nearest)
                        .setNormalizedCoordinates(false);

    auto imageView = image->createImageView(imgViewDesc);
    REQUIRE(imageView != nullptr);


    auto program = session->createProgram(SHADER_PATH + "/imgToBuffer.spv");
    REQUIRE(program != nullptr);

    auto nodeDescriptor = ll::ComputeNodeDescriptor()
                            .setProgram(program)
                            .setFunctionName("main")
                            .setLocalX(32)
                            .setLocalY(32)
                            .addImageViewParameter()
                            .addBufferParameter();

    auto node = session->createComputeNode(nodeDescriptor);
    REQUIRE(node != nullptr);

    auto cmdBuffer = session->createCommandBuffer();
    REQUIRE(cmdBuffer != nullptr);

    cmdBuffer->begin();
    cmdBuffer->changeImageLayout(*image, vk::ImageLayout::eTransferDstOptimal);
    cmdBuffer->copyBufferToImage(*stageBuffer, *image);
    cmdBuffer->changeImageLayout(*image, vk::ImageLayout::eGeneral);

    // the image view can only be bound after the underlying
    // image is in the correct layout.
    node->bind(0, imageView);
    node->bind(1, outputBuffer);

    cmdBuffer->run(*node);

    cmdBuffer->end();

    session->run(cmdBuffer);

    // END OF EXECUTION

    bufMapped = static_cast<uint8_t*>(stageBuffer->map());
    auto outMapped = static_cast<uint32_t*>(outputBuffer->map());

    for (auto row = 0u; row < HEIGHT; ++ row) {
        for (auto col = 0u; col < WIDTH; ++ col) {
            const auto coord = row*HEIGHT + col;

            REQUIRE(static_cast<uint32_t>(bufMapped[coord]) == outMapped[coord]);
            std::cout << outMapped[row*HEIGHT + col] << " ";
        }
        std::cout << std::endl;
    }

    outputBuffer->unmap();
    stageBuffer->unmap();

}


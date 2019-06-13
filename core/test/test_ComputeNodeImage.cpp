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


TEST_CASE("textureToBuffer", "test_ComputeNodeImage") {

    std::cout << "test_ComputeNodeImage: textureToBuffer" << std::endl;

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
                    .setChannelCount(ll::ChannelCount::C1)
                    .setUsageFlags(imgUsageFlags);

    auto stageBuffer  = hostMemory->createBuffer(imgDesc.getSize());
    auto outputBuffer = hostOutputMemory->createBuffer(imgDesc.getSize()*sizeof(uint32_t));

    {
        auto bufMapped = stageBuffer->map<uint8_t[]>();

        // copy image data
        for (auto row = 0u; row < HEIGHT; ++ row) {
            for (auto col = 0u; col < WIDTH; ++ col) {
                bufMapped[row*HEIGHT + col] = std::min(1u, (col + (row % 2)) % 2);
            }
        }
    }

    auto image = deviceMemory->createImage(imgDesc);

    auto imgViewDesc = ll::ImageViewDescriptor {}
                        .setNormalizedCoordinates(false)
                        .setIsSampled(true)
                        .setAddressMode(ll::ImageAddressMode::Repeat)
                        .setFilterMode(ll::ImageFilterMode::Nearest);

    auto imageView = image->createImageView(imgViewDesc);
    REQUIRE(imageView != nullptr);


    auto program = session->createProgram(SHADER_PATH + "/textureToBuffer.spv");
    REQUIRE(program != nullptr);

    auto nodeDescriptor = ll::ComputeNodeDescriptor()
                            .setProgram(program)
                            .setFunctionName("main")
                            .setLocalX(32)
                            .setLocalY(32)
                            .addPort({0, "in", ll::PortDirection::IN, ll::PortType::SampledImageView})
                            .addPort({1, "out", ll::PortDirection::OUT, ll::PortType::Buffer});

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

    session->run(*cmdBuffer);

    // END OF EXECUTION

    {
        auto bufMapped = stageBuffer->map<uint8_t[]>();
        auto outMapped = outputBuffer->map<uint32_t[]>();

        for (auto row = 0u; row < HEIGHT; ++ row) {
            for (auto col = 0u; col < WIDTH; ++ col) {
                const auto coord = row*HEIGHT + col;

                REQUIRE(static_cast<uint32_t>(bufMapped[coord]) == outMapped[coord]);
                std::cout << outMapped[coord] << " ";
            }
            std::cout << std::endl;
        }
    } // unmap bufMapped and outMapped

}


TEST_CASE("imageToBuffer", "test_ComputeNodeImage") {

    std::cout << "test_ComputeNodeImage: imageToBuffer" << std::endl;

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
                    .setChannelCount(ll::ChannelCount::C1)
                    .setUsageFlags(imgUsageFlags);

    auto stageBuffer  = hostMemory->createBuffer(imgDesc.getSize());
    auto outputBuffer = hostOutputMemory->createBuffer(imgDesc.getSize()*sizeof(uint32_t));

    {
        auto bufMapped = stageBuffer->map<uint8_t[]>();

        // copy image data
        for (auto row = 0u; row < HEIGHT; ++ row) {
            for (auto col = 0u; col < WIDTH; ++ col) {
                bufMapped[row*HEIGHT + col] = std::min(1u, (col + (row % 2)) % 2);
            }
        }
    } // unamp bufMapped

    auto image = deviceMemory->createImage(imgDesc);

    // image view descriptor for a storage image
    auto imgViewDesc = ll::ImageViewDescriptor {}
                        .setNormalizedCoordinates(false)
                        .setIsSampled(false);

    auto imageView = image->createImageView(imgViewDesc);
    REQUIRE(imageView != nullptr);


    auto program = session->createProgram(SHADER_PATH + "/imageToBuffer.spv");
    REQUIRE(program != nullptr);

    auto nodeDescriptor = ll::ComputeNodeDescriptor()
                            .setProgram(program)
                            .setFunctionName("main")
                            .setLocalX(32)
                            .setLocalY(32)
                            .addPort({0, "in", ll::PortDirection::IN, ll::PortType::ImageView})
                            .addPort({1, "out", ll::PortDirection::OUT, ll::PortType::Buffer});

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

    session->run(*cmdBuffer);

    // END OF EXECUTION

    {
        auto bufMapped = stageBuffer->map<uint8_t[]>();
        auto outMapped = outputBuffer->map<uint32_t[]>();

        for (auto row = 0u; row < HEIGHT; ++ row) {
            for (auto col = 0u; col < WIDTH; ++ col) {
                const auto coord = row*HEIGHT + col;

                REQUIRE(static_cast<uint32_t>(bufMapped[coord]) == outMapped[coord]);
                std::cout << outMapped[coord] << " ";
            }
            std::cout << std::endl;
        }
    } // unmap bufMapped and outMapped

}

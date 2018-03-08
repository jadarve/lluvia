/**
 * \file test_ComputeNode.cpp
 * \brief test compute node.
 * \copyright 2017, Juan David Adarve. See AUTHORS for more details
 * \license Apache 2.0, see LICENSE for more details
 */

#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"

#include "coreTestConfig.h"

#include <iostream>
#include "lluvia/core.h"


TEST_CASE("BufferAssignment", "test_ComputeNodeImage") {

    using memflags = vk::MemoryPropertyFlagBits;

    auto session = ll::Session::create();
    REQUIRE(session != nullptr);

    const auto hostMemFlags = memflags::eHostVisible | memflags::eHostCoherent;
    const auto deviceMemFlags = memflags::eDeviceLocal;
    const auto pageSize = uint64_t {1024*1024*4};

    auto hostMemory = session->createMemory(hostMemFlags, pageSize, false);
    REQUIRE(hostMemory != nullptr);

    auto deviceMemory = session->createMemory(deviceMemFlags, pageSize, false);
    REQUIRE(deviceMemory != nullptr);

    // stage buffer (host visible)
    // device image
    // copy stage buffer to device image

    auto imgDesc = ll::ImageDescriptor {}
                    .setWidth(32)
                    .setHeight(32)
                    .setChannelType(ll::ChannelType::Uint8)
                    .setChannelCount(1);

    auto stageBuffer  = hostMemory->createBuffer(imgDesc.getSize());
    auto outputBuffer = hostMemory->createBuffer(imgDesc.getSize());

    auto bufMapped = static_cast<uint8_t*>(stageBuffer->map());

    // copy image data
    
    stageBuffer->unmap();

    auto image = deviceMemory->createImage(imgDesc);

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
                            .addCombinedImageSamplerParameter() // addImageViewParameter()
                            .addBufferParameter();

    auto node = session->createComputeNode(nodeDescriptor);
    REQUIRE(node != nullptr);

    node->bind(0, imageView);
    node->bind(1, outputBuffer);

    session->run(node);

    // END OF STATIC PART


    auto cmdBuffer = session->createCommandBuffer();
    REQUIRE(cmdBuffer != nullptr);

    // transition image to transfer destination
    // copy from stage buffer to image
    // transition image to general? or other layout
    // run command buffer


    // session->run(cmdBuffer);

    // END OF EXECUTION

    auto outMapped = static_cast<uint32_t*>(outputBuffer->map());

    outputBuffer->unmap();

}


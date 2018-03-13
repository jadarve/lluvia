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


using memflags = vk::MemoryPropertyFlagBits;


TEST_CASE("WriteGraph_MemoryAndBuffers", "test_IO") {

    auto session = ll::Session::create();
    REQUIRE(session != nullptr);

    auto hostMemory = session->createMemory(memflags::eHostVisible | memflags::eHostCoherent, 1024, false);
    REQUIRE(hostMemory != nullptr);

    auto devMemory = session->createMemory(memflags::eDeviceLocal, 4096, false);
    REQUIRE(devMemory != nullptr);

    auto hostBuffer_0 = hostMemory->createBuffer(512);  REQUIRE(hostBuffer_0 != nullptr);
    auto hostBuffer_1 = hostMemory->createBuffer(1024); REQUIRE(hostBuffer_1 != nullptr);
    auto hostBuffer_2 = hostMemory->createBuffer(256);  REQUIRE(hostBuffer_2 != nullptr);

    auto devBuffer_0 = devMemory->createBuffer(128);  REQUIRE(devBuffer_0 != nullptr);
    auto devBuffer_1 = devMemory->createBuffer(32);   REQUIRE(devBuffer_1 != nullptr);
    auto devBuffer_2 = devMemory->createBuffer(768);  REQUIRE(devBuffer_2 != nullptr);
    auto devBuffer_3 = devMemory->createBuffer(2048); REQUIRE(devBuffer_3 != nullptr);
    auto devBuffer_4 = devMemory->createBuffer(4096); REQUIRE(devBuffer_4 != nullptr);

    auto graph = std::make_shared<ll::ComputeGraph>();
    graph->addMemory("hostMemory", hostMemory);
    graph->addMemory("devMemory", devMemory);

    graph->addObject("hostBuffer_0", hostBuffer_0);
    graph->addObject("hostBuffer_1", hostBuffer_1);
    graph->addObject("hostBuffer_2", hostBuffer_2);

    graph->addObject("devBuffer_0", devBuffer_0);
    graph->addObject("devBuffer_1", devBuffer_1);
    graph->addObject("devBuffer_2", devBuffer_2);
    graph->addObject("devBuffer_3", devBuffer_3);
    graph->addObject("devBuffer_4", devBuffer_4);

    auto program = session->createProgram(SHADER_PATH + "/assign.spv");
    REQUIRE(program != nullptr);
    graph->addProgram("assign", program);

    auto nodeDescriptor = ll::ComputeNodeDescriptor()
                            .setProgram(program)
                            .setFunctionName("main")
                            .setLocalX(1024)
                            .addBufferParameter();

    auto node = session->createComputeNode(nodeDescriptor);
    REQUIRE(node != nullptr);
    
    node->bind(0, hostBuffer_0);
    
    graph->addComputeNode("node_0", node);


    ll::writeComputeGraph(graph, "WriteGraph_MemoryAndBuffers");
}


TEST_CASE("WriteGraph_ImageAndImageView", "test_IO") {

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

    auto hostMemory       = session->createMemory(hostMemFlags, pageSize, false);
    auto hostOutputMemory = session->createMemory(hostMemFlags, pageSize, false);
    auto deviceMemory     = session->createMemory(deviceMemFlags, pageSize, false);

    REQUIRE(hostMemory != nullptr);
    REQUIRE(hostOutputMemory != nullptr);
    REQUIRE(deviceMemory != nullptr);

    auto imgDesc = ll::ImageDescriptor {}
                    .setWidth(WIDTH)
                    .setHeight(HEIGHT)
                    .setChannelType(ll::ChannelType::Uint8)
                    .setChannelCount(1);

    auto stageBuffer  = hostMemory->createBuffer(imgDesc.getSize());
    auto outputBuffer = hostOutputMemory->createBuffer(imgDesc.getSize()*sizeof(uint32_t));


    auto image = deviceMemory->createImage(imgDesc, imgUsageFlags);
    REQUIRE(image != nullptr);

    session->changeImageLayout(image, vk::ImageLayout::eGeneral);

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

    node->bind(0, imageView);
    node->bind(1, outputBuffer);

    auto graph = std::make_shared<ll::ComputeGraph>();
    
    graph->addMemory("hostMemory", hostMemory);
    graph->addMemory("hostOutputMemory", hostOutputMemory);
    graph->addMemory("deviceMemory", deviceMemory);

    graph->addObject("stageBuffer", stageBuffer);
    graph->addObject("outputBuffer", outputBuffer);
    graph->addObject("image", image);
    graph->addObject("imageView", imageView);

    graph->addProgram("imgToBuffer", program);
    graph->addComputeNode("node", node);

    ll::writeComputeGraph(graph, "moni moni");
}


TEST_CASE("ReadGraph_Buffers", "test_IO") {

    auto session = std::shared_ptr<ll::Session> {ll::Session::create()};
    REQUIRE(session != nullptr);

    auto graph = ll::readComputeGraph(DATA_PATH + "/buffers.json", session);
    REQUIRE(graph != nullptr);

    // check the contents of the graph
    REQUIRE(graph->containsMemory("devMemory"));
    REQUIRE(graph->containsMemory("hostMemory"));

    REQUIRE(graph->containsObject("devBuffer_0"));
    REQUIRE(graph->containsObject("devBuffer_1"));
    REQUIRE(graph->containsObject("devBuffer_2"));
    REQUIRE(graph->containsObject("devBuffer_3"));
    REQUIRE(graph->containsObject("devBuffer_4"));
    REQUIRE(graph->containsObject("hostBuffer_0"));
    REQUIRE(graph->containsObject("hostBuffer_1"));
    REQUIRE(graph->containsObject("hostBuffer_2"));
}


TEST_CASE("ReadGraph_ComputeNode", "test_IO") {

    auto session = std::shared_ptr<ll::Session> {ll::Session::create()};
    REQUIRE(session != nullptr);

    auto graph = ll::readComputeGraph(DATA_PATH + "/computeNode.json", session);
    REQUIRE(graph != nullptr);

    // check the contents of the graph
    REQUIRE(graph->containsMemory("hostMemory"));
    REQUIRE(graph->containsObject("hostBuffer_0"));
    REQUIRE(graph->containsProgram("assign"));
    REQUIRE(graph->containsComputeNode("node_0"));
}


TEST_CASE("ReadGraph_ImageAndImageView", "test_IO") {

    auto session = std::shared_ptr<ll::Session> {ll::Session::create()};
    REQUIRE(session != nullptr);

    auto graph = ll::readComputeGraph(DATA_PATH + "/images.json", session);
    REQUIRE(graph != nullptr);

    // check the contents of the graph
    REQUIRE(graph->containsMemory("deviceMemory"));
    REQUIRE(graph->containsMemory("hostMemory"));
    REQUIRE(graph->containsMemory("hostOutputMemory"));

    REQUIRE(graph->containsObject("outputBuffer"));
    REQUIRE(graph->containsObject("stageBuffer"));
    REQUIRE(graph->containsObject("image"));
    REQUIRE(graph->containsObject("imageView"));
}

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

    auto hostMemory = session->createMemory(memflags::eHostVisible | memflags::eHostCoherent, 1024, true);
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


    ll::writeComputeGraph(graph, "moni moni");
}


TEST_CASE("ReadGraph", "test_IO") {

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

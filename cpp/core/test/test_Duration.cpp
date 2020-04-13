/**
 * \file test_Duration.cpp
 * \brief test compute node.
 * \copyright 2017, Juan David Adarve. See AUTHORS for more details
 * \license Apache 2.0, see LICENSE for more details
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include <iostream>
#include "lluvia/core.h"


TEST_CASE("DurationRecording", "test_Duration")
{

    using memflags = vk::MemoryPropertyFlagBits;

    auto session = ll::Session::create();
    REQUIRE(session != nullptr);

    const auto hostMemFlags = memflags::eHostVisible | memflags::eHostCoherent;
    auto hostMemory = session->createMemory(hostMemFlags, 1024 * 4, false);
    REQUIRE(hostMemory != nullptr);

    const auto bufferSize = 128;
    auto buffer = hostMemory->createBuffer(bufferSize * sizeof(float));

    auto program = session->createProgram("cpp/core/test/glsl/assign.spv");
    REQUIRE(program != nullptr);

    auto nodeDescriptor = ll::ComputeNodeDescriptor()
                              .setProgram(program)
                              .setFunctionName("main")
                              .setLocalX(bufferSize)
                              .addPort({0, "out_buffer", ll::PortDirection::Out, ll::PortType::Buffer});

    // at this point, the node's port binding table and
    // vulkan descriptor set is created. So, it is possible
    // to bind objects to the ports before calling node->init()
    auto node = session->createComputeNode(nodeDescriptor);
    REQUIRE(node != nullptr);

    node->bind("out_buffer", buffer);
    node->init();

    auto duration = session->createDuration();
    REQUIRE(duration != nullptr);

    auto cmdBuffer = session->createCommandBuffer();
    REQUIRE(cmdBuffer != nullptr);

    cmdBuffer->begin();
    cmdBuffer->durationStart(*duration);
    cmdBuffer->run(*node);
    cmdBuffer->durationEnd(*duration);
    cmdBuffer->end();

    session->run(*cmdBuffer);

    auto ns = duration->getDuration();
    
    std::cout << "NANOSECONDS: " << ns.count() << std::endl;
}

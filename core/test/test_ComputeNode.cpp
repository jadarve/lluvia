/**
 * \file test_ComputeNode.cpp
 * \brief test compute node.
 * \copyright 2017, Juan David Adarve. See AUTHORS for more details
 * \license Apache 2.0, see LICENSE for more details
 */

#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"

#include <iostream>
#include <gtest/gtest.h>
#include "lluvia/core.h"

TEST_CASE("Construction", "[test_ComputeNode]") {

    auto session = ll::Session::create();
    REQUIRE(session != nullptr);

    const auto memoryFlags = vk::MemoryPropertyFlagBits::eDeviceLocal;
    auto memory = session->createMemory(memoryFlags, 1024*4);
    REQUIRE(memory != nullptr);

    auto buffer = memory->createBuffer(32*sizeof(float));

    auto program = session->createProgram("/home/jadarve/git/lluvia/glsl/comp.spv");
    REQUIRE(program != nullptr);

    auto nodeDescriptor = ll::ComputeNodeDescriptor()
                            .setProgram(program)
                            .setFunctionName("main")
                            .setLocalX(32)
                            .addBufferParameter();

    
     auto node = session->createComputeNode(nodeDescriptor);
     REQUIRE(node != nullptr);

//     node.bind(0, buffer);
//     session->run(node);
}


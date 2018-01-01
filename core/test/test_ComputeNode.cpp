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
#include <gtest/gtest.h>
#include "lluvia/core.h"


// TEST_CASE("Construction", "test_ComputeNode") {

//     auto session = ll::Session::create();
//     REQUIRE(session != nullptr);

//     const auto memoryFlags = vk::MemoryPropertyFlagBits::eDeviceLocal;
//     auto memory = session->createMemory(memoryFlags, 1024*4, false);
//     REQUIRE(memory != nullptr);

//     auto buffer = uniqueToShared(memory->createBuffer(32*sizeof(float)));
//     REQUIRE(buffer != nullptr);

//     auto program = session->createProgram("/home/jadarve/git/lluvia/glsl/comp.spv");
//     REQUIRE(program != nullptr);

//     auto nodeDescriptor = ll::ComputeNodeDescriptor()
//                             .setProgram(program)
//                             .setFunctionName("main")
//                             .setLocalX(32)
//                             .addBufferParameter();

//     auto node = uniqueToShared(session->createComputeNode(nodeDescriptor));
//     REQUIRE(node != nullptr);

//     node->bind(0, buffer);
//     session->run(node);
// }


TEST_CASE("BufferAssignment", "test_ComputeNode") {

    using memflags = vk::MemoryPropertyFlagBits;

    auto session = ll::Session::create();
    REQUIRE(session != nullptr);

    const auto hostMemFlags = memflags::eHostVisible | memflags::eHostCoherent;
    auto hostMemory = session->createMemory(hostMemFlags, 1024*4, false);
    REQUIRE(hostMemory != nullptr);

    const auto bufferSize = 32;
    auto buffer = hostMemory->createBuffer(bufferSize*sizeof(float));

    auto program = session->createProgram(SHADER_PATH + "/assign.spv");
    REQUIRE(program != nullptr);

    auto nodeDescriptor = ll::ComputeNodeDescriptor()
                            .setProgram(program)
                            .setFunctionName("main")
                            .setLocalX(32)
                            .addBufferParameter();

    auto node = session->createComputeNode(nodeDescriptor);
    REQUIRE(node != nullptr);

    node->bind(0, buffer);
    session->run(node);

}


TEST_CASE("ConstructionCommandBuffer", "test_ComputeNode") {

    // auto session = ll::Session::create();

    // const auto memoryFlags = vk::MemoryPropertyFlagBits::eDeviceLocal;
    // auto memory = session->createMemory(memoryFlags, 1024*sizeof(float));

    // auto buffer0      = uniqueToShared(memory->createBuffer(32*sizeof(float)));
    // auto buffer1      = uniqueToShared(memory->createBuffer(32*sizeof(float)));
    // auto bufferSubOut = uniqueToShared(memory->createBuffer(32*sizeof(float)));
    // auto bufferAbsOut = uniqueToShared(memory->createBuffer(32*sizeof(float)));

    // auto program = session->createProgram("/home/jadarve/git/lluvia/glsl/comp.spv");

    // auto desc0 = ll::ComputeNodeDescriptor()
    //                         .setProgram(program)
    //                         .setFunctionName("sub")
    //                         .setLocalX(32)
    //                         .addBufferParameter()   // first input
    //                         .addBufferParameter()   // second input
    //                         .addBufferParameter();  // output

    // auto desc1 = ll::ComputeNodeDescriptor()
    //                         .setProgram(program)
    //                         .setFunctionName("abs")
    //                         .setLocalX(32)
    //                         .addBufferParameter()   // input
    //                         .addBufferParameter();  // output

    // auto nodeSub = uniqueToShared(session->createComputeNode(desc0));
    // nodeSub->bind(0, buffer0);
    // nodeSub->bind(1, buffer1);
    // nodeSub->bind(2, bufferSubOut);

    // auto nodeAbs = uniqueToShared(session->createComputeNode(desc1));
    // nodeAbs->bind(0, bufferSubOut);
    // nodeAbs->bind(1, bufferAbsOut);

    // // create and record a command buffer
    // auto cmdBuffer = session->createCommandBuffer();
    // cmdBuffer->beginRecording();
    // nodeSub->record(cmdBuffer);
    // nodeAbs->record(cmdBuffer);
    // cmdBuffer->endRecording();

    // // run the command buffer
    // session->run(cmdBuffer);
}

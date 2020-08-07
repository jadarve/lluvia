/**
 * \file test_ComputeNode.cpp
 * \brief test compute node.
 * \copyright 2017, Juan David Adarve. See AUTHORS for more details
 * \license Apache 2.0, see LICENSE for more details
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include <cstdint>
#include <iostream>
#include "lluvia/core.h"


TEST_CASE("BufferAssignment", "test_ComputeNode") {

    constexpr const size_t length = 128;
    constexpr const size_t size = sizeof(float);

    using memflags = vk::MemoryPropertyFlagBits;

    auto session = ll::Session::create();
    REQUIRE(session != nullptr);

    const auto hostMemFlags = memflags::eHostVisible | memflags::eHostCoherent;
    auto hostMemory = session->createMemory(hostMemFlags, length * size, false);
    REQUIRE(hostMemory != nullptr);

    auto buffer = hostMemory->createBuffer(length*sizeof(float));
    REQUIRE(buffer != nullptr);

    auto program = session->createProgram("lluvia/cpp/core/test/glsl/assign.spv");
    REQUIRE(program != nullptr);

    auto nodeDescriptor = ll::ComputeNodeDescriptor()
                            .setProgram(program)
                            .setFunctionName("main")
                            .setLocalX(length)
                            .addPort({0, "out_buffer", ll::PortDirection::Out, ll::PortType::Buffer});

    // at this point, the node's port binding table and
    // vulkan descriptor set is created. So, it is possible
    // to bind objects to the ports before calling node->init()
    auto node = session->createComputeNode(nodeDescriptor);
    REQUIRE(node != nullptr);

    node->bind("out_buffer", buffer);
    node->init();

    auto cmdBuffer = session->createCommandBuffer();
    REQUIRE(cmdBuffer != nullptr);

    cmdBuffer->begin();
    cmdBuffer->run(*node);
    cmdBuffer->end();

    session->run(*cmdBuffer);    

    {
        auto bufferMap = buffer->map<float[]>();
        for (auto i = 0u; i < length; ++i) {
            REQUIRE(bufferMap[i] == static_cast<float>(i));
        }
    } // unamp bufferMap
}


TEST_CASE("ConstructWithInterpreter", "test_ComputeNode") {

    using memflags = vk::MemoryPropertyFlagBits;

    auto session = ll::Session::create();
    REQUIRE(session != nullptr);

    const auto hostMemFlags = memflags::eHostVisible | memflags::eHostCoherent;
    auto hostMemory = session->createMemory(hostMemFlags, 1024*4, false);
    REQUIRE(hostMemory != nullptr);

    const auto bufferSize = 128;
    auto buffer = hostMemory->createBuffer(bufferSize*sizeof(float));
    REQUIRE(buffer != nullptr);

    auto program = session->createProgram("lluvia/cpp/core/test/glsl/assign.spv");
    REQUIRE(program != nullptr);

    session->setProgram("assign", program);

    // register the node builder
    session->script(R"(
local builder = ll.class(ll.ComputeNodeBuilder)

function builder.newDescriptor() 
    
    local desc = ll.ComputeNodeDescriptor.new()
    
    desc.builderName  = 'assign'
    desc.localShape   = ll.vec3ui.new(32, 32, 1)
    desc.gridShape    = ll.vec3ui.new(1, 1, 1)
    desc.program      = ll.getProgram('assign')
    desc.functionName = 'main'

    desc:addPort(ll.PortDescriptor.new(0, 'out_buffer', ll.PortDirection.Out, ll.PortType.Buffer))

    return desc
end

function builder.onNodeInit(node)

    -- configure gridShape given the size of out_buffer
    local out_buffer = node:getPort('out_buffer')
    
    local N = out_buffer.size // 4
    node:configureGridShape(ll.vec3ui.new(N, 1, 1))

end

ll.registerNodeBuilder('assign', builder)
        )");

    auto desc = session->createComputeNodeDescriptor("assign");

    std::cout << "functionName: " << desc.getFunctionName() << std::endl;
    std::cout << "builderName: " << desc.getBuilderName() << std::endl;
    std::cout << "local: " << desc.getLocalX() << ", " << desc.getLocalY() << ", " << desc.getLocalZ() << std::endl;
    std::cout << "grid: " << desc.getGridX() << ", " << desc.getGridY() << ", " << desc.getGridZ() << std::endl;

    auto node = session->createComputeNode(desc);
    REQUIRE(node != nullptr);

    node->bind("out_buffer", buffer);
    node->init();

    std::cout << "node grid: " << node->getGridX() << ", " << node->getGridY() << ", " << node->getGridZ() << std::endl;


    auto cmdBuffer = session->createCommandBuffer();

    cmdBuffer->begin();
    cmdBuffer->run(*node);
    cmdBuffer->end();

    session->run(*cmdBuffer);

    {
        auto bufferMap = buffer->map<float[]>();
        for (auto i = 0u; i < bufferSize; ++i) {
            REQUIRE(bufferMap[i] == static_cast<float>(i));
        }
    } // unamp bufferMap
}
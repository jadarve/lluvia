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

    const auto bufferSize = 128;
    auto buffer = hostMemory->createBuffer(bufferSize*sizeof(float));

    auto program = session->createProgram(SHADER_PATH + "/assign.spv");
    REQUIRE(program != nullptr);

    // TOTHINK: set script in the descriptor, not in the node
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

    // these are equivalent
    node->init();
    // node->setState(ll::NodeState::Init);

    auto cmdBuffer = session->createCommandBuffer();

    cmdBuffer->begin();
    cmdBuffer->run(*node);
    cmdBuffer->end();

    session->run(*cmdBuffer);
    

    {
        auto bufferMap = buffer->map<float[]>();
        for (auto i = 0u; i < bufferSize; ++i) {

            std::cout << i << ": " << bufferMap[i] << std::endl;;
        }
    } // unamp bufferMap
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

TEST_CASE("ConstructWithInterpreter", "test_ComputeNode") {

    using memflags = vk::MemoryPropertyFlagBits;

    auto session = ll::Session::create();
    REQUIRE(session != nullptr);

    const auto hostMemFlags = memflags::eHostVisible | memflags::eHostCoherent;
    auto hostMemory = session->createMemory(hostMemFlags, 1024*4, false);
    REQUIRE(hostMemory != nullptr);

    const auto bufferSize = 128;
    auto buffer = hostMemory->createBuffer(bufferSize*sizeof(float));

    auto program = session->createProgram(SHADER_PATH + "/assign.spv");
    REQUIRE(program != nullptr);

    // registerProgram?
    session->setProgram("assign", program);

    // register the node builder
    session->script(R"(
local builder = ll.class(ll.ComputeNodeBuilder)

function builder.newDescriptor() 
    
    local desc = ll.ComputeNodeDescriptor.new()
    
    desc.builderName = 'assign'
    desc.localShape = ll.vec3ui.new(32, 1, 1)
    desc.gridShape = ll.vec3ui.new(1, 1, 1)
    
    -- desc.program = ll.getProgram('assign')
    desc.functionName = 'main'

    desc:addPort(ll.PortDescriptor.new(0, 'out_buffer', ll.PortDirection.Out, ll.PortType.Buffer))

    return desc
end

function builder.onNodeInit(node)

    print('assign.onNodeInit')
    print(type(node))

    -- configure gridShape given the size of out_buffer
    out_buffer = ll.castObject(node:getPort('out_buffer'))
    
    N = out_buffer.size // 4
    print('N', N)
    node:configureGridShape(ll.vec3ui.new(N, 1, 1))

    -- I can also bind objects here
    print('assign.onNodeInit : finish')
end

ll.registerNodeBuilder('assign', builder)
        )");

    auto desc = session->createComputeNodeDescriptor("assign");
    desc.setProgram(program);

    std::cout << "functionName: " << desc.getFunctionName() << std::endl;
    std::cout << "builderName: " << desc.getBuilderName() << std::endl;
    std::cout << "local: " << desc.getLocalX() << ", " << desc.getLocalY() << ", " << desc.getLocalZ() << std::endl;
    std::cout << "grid: " << desc.getGridX() << ", " << desc.getGridY() << ", " << desc.getGridZ() << std::endl;

    auto node = session->createComputeNode(desc);
    node->bind("out_buffer", buffer);
    node->init();

    std::cout << "node grid: " << node->getGridX() << ", " << node->getGridY() << ", " << node->getGridZ() << std::endl;



    // // TOTHINK: set script in the descriptor, not in the node
    // auto nodeDescriptor = ll::ComputeNodeDescriptor()
    //                         .setProgram(program)
    //                         .setFunctionName("main")
    //                         .setLocalX(bufferSize)
    //                         .addPort({0, "out_buffer", ll::PortDirection::Out, ll::PortType::Buffer});

    // // at this point, the node's port binding table and
    // // vulkan descriptor set is created. So, it is possible
    // // to bind objects to the ports before calling node->init()
    // auto node = session->createComputeNode(nodeDescriptor);
    // REQUIRE(node != nullptr);

    // node->bind("out_buffer", buffer);

    // // these are equivalent
    // node->init();
    // // node->setState(ll::NodeState::Init);

    // auto cmdBuffer = session->createCommandBuffer();

    // cmdBuffer->begin();
    // cmdBuffer->run(*node);
    // cmdBuffer->end();

    // session->run(*cmdBuffer);
    

    // {
    //     auto bufferMap = buffer->map<float[]>();
    //     for (auto i = 0u; i < bufferSize; ++i) {

    //         std::cout << i << ": " << bufferMap[i] << std::endl;;
    //     }
    // } // unamp bufferMap
}
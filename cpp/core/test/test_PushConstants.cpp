/**
@file       test_BufferCopy.h
@brief      Test buffer mapping operations.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include <cstdint>
#include <iostream>
#include <system_error>

#include "lluvia/core.h"

TEST_CASE("Creation", "test_PushConstants") {

    constexpr auto a = int32_t {789456};
    constexpr auto b = float {3.1415f};

    using params = struct {
        int32_t a;
        float b;

    };

    auto c = ll::PushConstants{};

    c.set(params{a, b});
    REQUIRE(c.getSize() == 8);

    auto cOut = c.get<params>();
    REQUIRE(cOut.a == a);
    REQUIRE(cOut.b == b);

    c.set(int32_t{456});
    REQUIRE(c.getSize() == 4);

    auto cInt = c.get<int32_t>();
    REQUIRE(cInt == 456);
}


TEST_CASE("BadSize", "test_PushConstants") {

    using params = struct
    {
        int32_t a;
        float b;
    };

    auto c = ll::PushConstants{};
    c.set(params{0, 3.1415f});

    // c contains a struct of size 8 bytes, while int32_t is only 4
    REQUIRE_THROWS_AS(c.get<int32_t>(), std::system_error);

}


TEST_CASE("ComputeNode", "test_PushConstants") {

    constexpr const float constantValue = 3.1415f;
    constexpr const size_t N{32};

    auto session = ll::Session::create();
    REQUIRE(session != nullptr);

    auto constants = ll::PushConstants{};
    constants.setFloat(3.1415f);

    auto program = session->createProgram("cpp/core/test/glsl/pushConstants.spv");

    auto desc = ll::ComputeNodeDescriptor{}
        .setFunctionName("main")
        .setProgram(program)
        .setGridShape({N / 32, 1, 1})
        .setLocalShape({32, 1, 1})
        .addPort({0, "out_buffer", ll::PortDirection::Out, ll::PortType::Buffer})
        .setPushConstants(constants);
    
    auto node = session->createComputeNode(desc);
    REQUIRE(node != nullptr);
    
    auto buffer = session->getHostMemory()->createBuffer(N * sizeof(constantValue));
    REQUIRE(buffer != nullptr);

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
        for (auto i = 0u; i < N; ++i) {
            REQUIRE(bufferMap[i] == constantValue);
        }
    }
}

TEST_CASE("Push2Constants", "test_PushConstants")
{

    constexpr const float firstValue  = 3.1415f;
    constexpr const float secondValue = 0.7896f;
    constexpr const size_t N{32};

    auto session = ll::Session::create();
    REQUIRE(session != nullptr);

    auto constants = ll::PushConstants{};
    constants.pushFloat(firstValue);
    constants.pushFloat(secondValue);

    auto program = session->createProgram("cpp/core/test/glsl/pushConstants2.spv");

    auto desc = ll::ComputeNodeDescriptor{}
                    .setFunctionName("main")
                    .setProgram(program)
                    .setGridShape({N / 32, 1, 1})
                    .setLocalShape({32, 1, 1})
                    .addPort({0, "out_buffer", ll::PortDirection::Out, ll::PortType::Buffer})
                    .setPushConstants(constants);

    auto node = session->createComputeNode(desc);
    REQUIRE(node != nullptr);

    auto buffer = session->getHostMemory()->createBuffer(N * sizeof(firstValue));
    REQUIRE(buffer != nullptr);

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
        for (auto i = 0u; i < N; ++i) {

            if (i % 2 == 0) {
                REQUIRE(bufferMap[i] == firstValue);
            } else {
                REQUIRE(bufferMap[i] == secondValue);
            }
            
        }
    }
}

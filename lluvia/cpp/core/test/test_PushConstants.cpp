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

#include "tools/cpp/runfiles/runfiles.h"
using bazel::tools::cpp::runfiles::Runfiles;

TEST_CASE("Creation", "test_PushConstants")
{

    constexpr auto a = int32_t {789456};
    constexpr auto b = float {3.1415f};

    using params = struct {
        int32_t a;
        float   b;
    };

    auto pushConstants = ll::PushConstants {};

    pushConstants.set(params {a, b});
    REQUIRE(pushConstants.getSize() == 8);

    auto cOut = pushConstants.get<params>();
    REQUIRE(cOut.a == a);
    REQUIRE(cOut.b == b);

    pushConstants.set(int32_t {456});
    REQUIRE(pushConstants.getSize() == 4);

    auto cInt = pushConstants.get<int32_t>();
    REQUIRE(cInt == 456);
}

TEST_CASE("BadSize", "test_PushConstants")
{

    using params = struct
    {
        int32_t a;
        float   b;
    };

    auto pushConstants = ll::PushConstants {};
    pushConstants.set(params {0, 3.1415f});

    REQUIRE(pushConstants.getSize() == 8);

    // pushConstants contains a struct of size 8 bytes, while int32_t is only 4
    REQUIRE_THROWS_AS(pushConstants.get<int32_t>(), std::system_error);
}

TEST_CASE("ComputeNode", "test_PushConstants")
{

    auto runfiles = Runfiles::CreateForTest(nullptr);
    REQUIRE(runfiles != nullptr);

    constexpr const float  constantValue = 3.1415f;
    constexpr const size_t N {32};

    auto session = ll::Session::create(ll::SessionDescriptor().enableDebug(true));
    REQUIRE(session != nullptr);

    auto constants = ll::PushConstants {};
    constants.setFloat(3.1415f);
    REQUIRE(constants.getSize() == 4);

    auto program = session->createProgram(runfiles->Rlocation("lluvia/lluvia/cpp/core/test/glsl/pushConstants.comp.spv"));

    auto desc = ll::ComputeNodeDescriptor {}
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

    REQUIRE_FALSE(session->hasReceivedVulkanWarningMessages());
}

TEST_CASE("Push2Constants", "test_PushConstants")
{

    auto runfiles = Runfiles::CreateForTest(nullptr);
    REQUIRE(runfiles != nullptr);

    constexpr const float  firstValue  = 3.1415f;
    constexpr const float  secondValue = 0.7896f;
    constexpr const size_t N {32};

    auto session = ll::Session::create(ll::SessionDescriptor().enableDebug(true));
    REQUIRE(session != nullptr);

    auto constants = ll::PushConstants {};
    constants.pushFloat(firstValue);
    constants.pushFloat(secondValue);
    REQUIRE(constants.getSize() == 8);

    auto program = session->createProgram(runfiles->Rlocation("lluvia/lluvia/cpp/core/test/glsl/pushConstants2.comp.spv"));

    auto desc = ll::ComputeNodeDescriptor {}
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

    REQUIRE_FALSE(session->hasReceivedVulkanWarningMessages());
}

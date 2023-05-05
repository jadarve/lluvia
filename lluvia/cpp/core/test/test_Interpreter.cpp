/**
 * \file test_Base64.cpp
 * \brief test base 64 encoding and decoding.
 * \copyright 2017, Juan David Adarve. See AUTHORS for more details
 * \license Apache 2.0, see LICENSE for more details
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "lluvia/core.h"

#include <memory>
#include <string>

TEST_CASE("test_error_unknown_method", "test_Interpreter")
{

    auto interpreter = std::make_unique<ll::Interpreter>();

    constexpr auto lua = R"(
        stringParam = ...

        function getNode(builderName) 
            local desc = ll.ComputeNodeDescriptor.new()
            desc.builderName  = stringParam

            -- this function should trigger an error
            desc:UnknownFunction()
            return desc
        end

        return getNode(stringParam)
    )";

    REQUIRE_THROWS_AS(interpreter->loadAndRun<ll::ComputeNodeDescriptor>(lua, "stringParamValue"), std::system_error);
}

TEST_CASE("test_error_unknown_method_from_session", "test_Interpreter")
{

    auto session = ll::Session::create(ll::SessionDescriptor().enableDebug(true));
    REQUIRE(session != nullptr);

    constexpr auto lua = R"(
        local builder = ll.class(ll.ComputeNodeBuilder)
        builder.name = 'TestNode'

        function builder.newDescriptor() 
            
            local desc = ll.ComputeNodeDescriptor.new()
            
            desc.builderName  = 'TestNode'
            desc.localShape   = ll.vec3ui.new(32, 32, 1)
            desc.gridShape    = ll.vec3ui.new(1, 1, 1)
            desc.functionName = 'main'

            -- this function should trigger an error
            desc:UnknownFunction()

            return desc
        end

        function builder.onNodeInit(node)
            ll.logd('TestNode', 'onNodeInit')
        end

        ll.registerNodeBuilder(builder)

    )";

    REQUIRE_NOTHROW(session->script(lua));

    REQUIRE_THROWS_AS(session->createComputeNodeDescriptor("TestNode"), std::system_error);
    REQUIRE_FALSE(session->hasReceivedVulkanWarningMessages());
}

TEST_CASE("test_non_existing_file", "test_Interpreter")
{

    auto session = ll::Session::create(ll::SessionDescriptor().enableDebug(true));
    REQUIRE(session != nullptr);

    REQUIRE_THROWS_AS(session->scriptFile("some_invalid_path.lua"), std::system_error);
    REQUIRE_FALSE(session->hasReceivedVulkanWarningMessages());
}

TEST_CASE("test_syntax_error", "test_Interpreter")
{
    auto session = ll::Session::create(ll::SessionDescriptor().enableDebug(true));
    REQUIRE(session != nullptr);

    constexpr auto lua = R"(

        function hello()
            -- missing closing quote and parenthesis
            print('hello world
        end

        hello()
    )";

    REQUIRE_THROWS_AS(session->script(lua), std::system_error);
    REQUIRE_FALSE(session->hasReceivedVulkanWarningMessages());
}

TEST_CASE("bufferMapAndSetFromBase64", "test_Interpreter")
{

    ///////////////////////////////////////////////////////
    // given a session and a Lua script

    auto session = ll::Session::create(ll::SessionDescriptor().enableDebug(true));
    REQUIRE(session != nullptr);

    constexpr auto lua = R"(

        local builder = ll.class(ll.ContainerNodeBuilder)
        builder.name = 'MapAndSetFromBase64'
        builder.doc = 'doc string'

        function builder.newDescriptor()

            local desc = ll.ContainerNodeDescriptor.new()

            desc.builderName = builder.name

            return desc
        end

        function builder.onNodeInit(node)

            local base64 = 'T3mZ8YSxEOdpxg=='
            local decodedData = ll.fromBase64(base64)
            local bufferSize = decodedData:size()

            local hostMemory = ll.getHostMemory()
            local stagingBuffer = hostMemory:createBuffer(bufferSize)
            stagingBuffer:mapAndSetFromVectorUint8(decodedData)

            node:bind('stagingBuffer', stagingBuffer)
        end

        function builder.onNodeRecord(node, cmdBuffer)
            -- do nothing
        end

        ll.registerNodeBuilder(builder)
    )";

    const auto data = std::vector<uint8_t> {79, 121, 153, 241, 132, 177, 16, 231, 105, 198};
    REQUIRE_NOTHROW(session->script(lua));

    ///////////////////////////////////////////////////////
    // when a container node is created
    auto node = session->createContainerNode("MapAndSetFromBase64");
    REQUIRE(node != nullptr);

    // and the node is initialized
    REQUIRE_NOTHROW(node->init());

    ///////////////////////////////////////////////////////
    // then the staging buffer is retrieved
    auto stagingBuffer = std::static_pointer_cast<ll::Buffer>(node->getPort("stagingBuffer"));
    REQUIRE(stagingBuffer != nullptr);

    // and the content of the staging buffer is the same as the original data
    {
        auto mapPtr = stagingBuffer->map<uint8_t[]>();
        REQUIRE(mapPtr != nullptr);

        for (auto i = 0u; i < data.size(); ++i) {
            REQUIRE(mapPtr[i] == data[i]);
        }
    }

    REQUIRE_FALSE(session->hasReceivedVulkanWarningMessages());
}

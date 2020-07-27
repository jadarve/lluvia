/**
 * \file test_Base64.cpp
 * \brief test base 64 encoding and decoding.
 * \copyright 2017, Juan David Adarve. See AUTHORS for more details
 * \license Apache 2.0, see LICENSE for more details
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "lluvia/core.h"

#include <string>
#include <memory>


TEST_CASE("test_error_unknown_method", "test_Interpreter") {

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

    auto session = ll::Session::create();
    REQUIRE(session != nullptr);

    constexpr auto lua = R"(
        local builder = ll.class(ll.ComputeNodeBuilder)

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

        ll.registerNodeBuilder('TestNode', builder)

    )";
    // auto interpreter = std::make_unique<ll::Interpreter>();

    session->script(lua);

    REQUIRE_THROWS_AS(session->createComputeNodeDescriptor("TestNode"), std::system_error);
    
    // REQUIRE_THROWS(interpreter->loadAndRun<ll::ComputeNodeDescriptor>(lua, "stringParamValue"));
}
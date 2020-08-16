/**
 * \file test_LoadLibrary.cpp
 * \brief test library load.
 * \copyright 2017, Juan David Adarve. See AUTHORS for more details
 * \license Apache 2.0, see LICENSE for more details
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include <cstdint>
#include <iostream>
#include "lluvia/core.h"


TEST_CASE("LoadLibrary", "test_LoadLibrary") {

    auto session = ll::Session::create();

    REQUIRE_NOTHROW(session->loadLibrary("lluvia/cpp/core/test/nodes/test_node_library.zip"));

    auto program = session->getProgram("nodes/Assign");
    REQUIRE(program != nullptr);

    auto desc = ll::ComputeNodeDescriptor{};
    REQUIRE_NOTHROW(desc = session->createComputeNodeDescriptor("nodes/Assign"));

    auto node = std::shared_ptr<ll::ComputeNode>{nullptr};
    REQUIRE_NOTHROW(node = session->createComputeNode(desc));
    REQUIRE(node != nullptr);
}

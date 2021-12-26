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

#include "tools/cpp/runfiles/runfiles.h"
using bazel::tools::cpp::runfiles::Runfiles;


TEST_CASE("LoadLibrary", "test_LoadLibrary") {

    auto runfiles = Runfiles::CreateForTest(nullptr);
    REQUIRE(runfiles != nullptr);

    auto session = ll::Session::create(ll::SessionDescriptor().enableDebug(true));

    REQUIRE_NOTHROW(session->loadLibrary(runfiles->Rlocation("lluvia/lluvia/cpp/core/test/nodes/test_node_library.zip")));

    auto program = session->getProgram("nodes/Assign.comp");
    REQUIRE(program != nullptr);

    auto desc = ll::ComputeNodeDescriptor{};
    REQUIRE_NOTHROW(desc = session->createComputeNodeDescriptor("nodes/Assign"));

    auto node = std::shared_ptr<ll::ComputeNode>{nullptr};
    REQUIRE_NOTHROW(node = session->createComputeNode(desc));
    REQUIRE(node != nullptr);

    REQUIRE_FALSE(ll::hasReceivedVulkanWarningMessages());
}

/**
 * \file test_MemoryBarrier.cpp
 * \brief test memory barrier creation
 * \copyright 2017, Juan David Adarve. See AUTHORS for more details
 * \license Apache 2.0, see LICENSE for more details
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include <iostream>
#include "lluvia/core.h"


TEST_CASE("EmptyCommandBuffer", "test_MemoryBarrier") {

    auto session = ll::Session::create(ll::SessionDescriptor().enableDebug(true));
    REQUIRE(session != nullptr);

    auto cmdBuffer = session->createCommandBuffer();
    REQUIRE(cmdBuffer != nullptr);

    cmdBuffer->begin();
    cmdBuffer->memoryBarrier();
    cmdBuffer->end();
    
    session->run(*cmdBuffer);

    REQUIRE_FALSE(ll::hasReceivedVulkanWarningMessages());
}

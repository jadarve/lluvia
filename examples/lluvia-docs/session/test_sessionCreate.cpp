#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include <iostream>
#include <memory>
#include "lluvia/core.h"


TEST_CASE("DebugEnabled", "test_sessionCreate")
{

    ll::SessionDescriptor desc = ll::SessionDescriptor().enableDebug(true);

    std::shared_ptr<ll::Session> session = ll::Session::create(desc);

    REQUIRE(session != nullptr);
    REQUIRE_FALSE(session->hasReceivedVulkanWarningMessages());
}

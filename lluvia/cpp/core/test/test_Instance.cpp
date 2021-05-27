/**
 * \file test_Instance.cpp
 * \brief test ll::vulkan::Instance class
 * \copyright 2017, Juan David Adarve. See AUTHORS for more details
 * \license Apache 2.0, see LICENSE for more details
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include <memory>
#include <iostream>

#include "lluvia/core.h"
#include "lluvia/core/vulkan/Instance.h"

TEST_CASE("create_instance", "test_Instance") {

    {
        auto instance = std::make_unique<ll::vulkan::Instance>(false);

        REQUIRE(instance != nullptr);
    }

    std::cout << "test_instance: finish" << std::endl;
}


TEST_CASE("create_instance_with_debug", "test_Instance") {

    auto instance = std::unique_ptr<ll::vulkan::Instance> {nullptr};

    // create instance with debug enabled
    REQUIRE_NOTHROW(instance = std::make_unique<ll::vulkan::Instance>(true));
    REQUIRE(instance != nullptr);

    REQUIRE_FALSE(ll::hasReceivedVulkanWarningMessages());
}

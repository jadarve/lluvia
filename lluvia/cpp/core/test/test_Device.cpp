/**
 * \file test_Device.cpp
 * \brief test the vulkan device.
 * \copyright 2022, Juan David Adarve. See AUTHORS for more details
 * \license Apache 2.0, see LICENSE for more details
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "lluvia/core.h"
#include "lluvia/core/vulkan/Device.h"

TEST_CASE("OptimalLocalShapeD1", "test_Device")
{
    const auto maxInvocations = 1024u;
    const auto maxSize        = ll::vec3ui {256, 1024, 2048};

    auto shape = ll::vulkan::computeOptimalLocalShape(ll::ComputeDimension::D1, maxInvocations, maxSize);

    REQUIRE(shape.x == maxSize.x);
    REQUIRE(shape.y == 1u);
    REQUIRE(shape.z == 1u);
}

TEST_CASE("OptimalLocalShapeD2_1024", "test_Device")
{
    const auto maxInvocations = 1024u;
    const auto maxSize        = ll::vec3ui {1024, 1024, 1024};

    auto shape = ll::vulkan::computeOptimalLocalShape(ll::ComputeDimension::D2, maxInvocations, maxSize);

    REQUIRE(shape.x == 32);
    REQUIRE(shape.y == 32);
    REQUIRE(shape.z == 1u);
}

TEST_CASE("OptimalLocalShapeD2_256", "test_Device")
{
    const auto maxInvocations = 256u;
    const auto maxSize        = ll::vec3ui {256, 256, 256};

    auto shape = ll::vulkan::computeOptimalLocalShape(ll::ComputeDimension::D2, maxInvocations, maxSize);

    REQUIRE(shape.x == 16);
    REQUIRE(shape.y == 16);
    REQUIRE(shape.z == 1u);
}

TEST_CASE("OptimalLocalShapeD3", "test_Device")
{
    const auto maxInvocations = 1024u;
    const auto maxSize        = ll::vec3ui {1024, 1024, 1024};

    auto shape = ll::vulkan::computeOptimalLocalShape(ll::ComputeDimension::D3, maxInvocations, maxSize);

    REQUIRE(shape.x == 16);
    REQUIRE(shape.y == 16);
    REQUIRE(shape.z == 4u);
}

TEST_CASE("OptimalLocalShapeD3_256", "test_Device")
{
    const auto maxInvocations = 256u;
    const auto maxSize        = ll::vec3ui {256, 256, 256};

    auto shape = ll::vulkan::computeOptimalLocalShape(ll::ComputeDimension::D3, maxInvocations, maxSize);

    REQUIRE(shape.x == 8);
    REQUIRE(shape.y == 8);
    REQUIRE(shape.z == 4u);
}

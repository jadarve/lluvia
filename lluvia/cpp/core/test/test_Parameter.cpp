/**
 * \file test_Parameter.cpp
 * \brief test parameter.
 * \copyright 2023, Juan David Adarve. See AUTHORS for more details
 * \license Apache 2.0, see LICENSE for more details
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "lluvia/core.h"

TEST_CASE("GoodUseNumericalTypes", "test_Parameter")
{

    auto p = ll::Parameter {};

    p.set(1);
    REQUIRE(p.getType() == ll::ParameterType::Int);
    REQUIRE(p.get<int>() == 1);

    p.set(1.0f);
    REQUIRE(p.getType() == ll::ParameterType::Float);
    REQUIRE(p.get<float>() == 1.0f);
}

TEST_CASE("BadUseNumericalTypes", "test_Parameter")
{

    auto p = ll::Parameter {};

    p.set(std::string {"hello"});

    REQUIRE_THROWS_AS(p.get<int>(), std::system_error);
}

TEST_CASE("GoodUseStringType", "test_Parameter")
{

    auto p = ll::Parameter {};

    p.set(std::string {"hello"});
    REQUIRE(p.getType() == ll::ParameterType::String);
    REQUIRE(p.get<std::string>() == "hello");
}

TEST_CASE("BadUseStringType", "test_Parameter")
{

    auto p = ll::Parameter {};

    p.set(1);

    REQUIRE_THROWS_AS(p.get<std::string>(), std::system_error);
}

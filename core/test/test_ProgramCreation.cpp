/**
 * \file test_SessionCreation.cpp
 * \brief test Session creation
 * \copyright 2017, Juan David Adarve. See AUTHORS for more details
 * \license Apache 2.0, see LICENSE for more details
 */

#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"

#include "coreTestConfig.h"

#include <iostream>
#include "lluvia/core.h"


TEST_CASE("NullProgram", "[test_ProgramCreation]") {

    auto session = ll::Session::create();
    REQUIRE(session != nullptr);

    REQUIRE_THROWS_AS(session->createProgram("noExist.spv"), std::system_error);
}


TEST_CASE("DefaultParameters", "[test_ProgramCreation]") {

    auto session = ll::Session::create();
    REQUIRE(session != nullptr);

    auto program = session->createProgram(SHADER_PATH + "/assign.spv");
    REQUIRE(program != nullptr);
}



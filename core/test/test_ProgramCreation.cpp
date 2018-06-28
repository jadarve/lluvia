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


/**
SPIR-V for assign.comp shader
*/
auto spirv = std::vector<uint8_t> {{
    3   , 2   , 35  , 7   , 0   , 0   , 1   , 0   , 2   , 0   , 13  , 0   , 32  , 0   , 0   , 0  , 0   , 0  , 0   , 0   , 17  , 0   , 2   , 0   , 1   , 0   , 0   , 0   , 11  , 0   ,
    6   , 0   , 1   , 0   , 0   , 0   , 71  , 76  , 83  , 76  , 46  , 115 , 116 , 100 , 46  , 52 , 53  , 48 , 0   , 0   , 0   , 0   , 14  , 0   , 3   , 0   , 0   , 0   , 0   , 0   ,
    1   , 0   , 0   , 0   , 15  , 0   , 6   , 0   , 5   , 0   , 0   , 0   , 4   , 0   , 0   , 0  , 109 , 97 , 105 , 110 , 0   , 0   , 0   , 0   , 11  , 0   , 0   , 0   , 16  , 0   ,
    6   , 0   , 4   , 0   , 0   , 0   , 17  , 0   , 0   , 0   , 1   , 0   , 0   , 0   , 1   , 0  , 0   , 0  , 1   , 0   , 0   , 0   , 3   , 0   , 3   , 0   , 2   , 0   , 0   , 0   ,
    194 , 1   , 0   , 0   , 4   , 0   , 10  , 0   , 71  , 76  , 95  , 71  , 79  , 79  , 71  , 76 , 69  , 95 , 99  , 112 , 112 , 95  , 115 , 116 , 121 , 108 , 101 , 95  , 108 , 105 ,
    110 , 101 , 95  , 100 , 105 , 114 , 101 , 99  , 116 , 105 , 118 , 101 , 0   , 0   , 4   , 0  , 8   , 0  , 71  , 76  , 95  , 71  , 79  , 79  , 71  , 76  , 69  , 95  , 105 , 110 ,
    99  , 108 , 117 , 100 , 101 , 95  , 100 , 105 , 114 , 101 , 99  , 116 , 105 , 118 , 101 , 0  , 5   , 0  , 4   , 0   , 4   , 0   , 0   , 0   , 109 , 97  , 105 , 110 , 0   , 0   ,
    0   , 0   , 5   , 0   , 4   , 0   , 8   , 0   , 0   , 0   , 105 , 110 , 100 , 101 , 120 , 0  , 0   , 0  , 5   , 0   , 8   , 0   , 11  , 0   , 0   , 0   , 103 , 108 , 95  , 71  ,
    108 , 111 , 98  , 97  , 108 , 73  , 110 , 118 , 111 , 99  , 97  , 116 , 105 , 111 , 110 , 73 , 68  , 0  , 0   , 0   , 5   , 0   , 4   , 0   , 18  , 0   , 0   , 0   , 111 , 117 ,
    116 , 48  , 0   , 0   , 0   , 0   , 6   , 0   , 7   , 0   , 18  , 0   , 0   , 0   , 0   , 0  , 0   , 0  , 111 , 117 , 116 , 112 , 117 , 116 , 66  , 117 , 102 , 102 , 101 , 114 ,
    0   , 0   , 0   , 0   , 5   , 0   , 3   , 0   , 20  , 0   , 0   , 0   , 0   , 0   , 0   , 0  , 71  , 0  , 4   , 0   , 11  , 0   , 0   , 0   , 11  , 0   , 0   , 0   , 28  , 0   ,
    0   , 0   , 71  , 0   , 4   , 0   , 17  , 0   , 0   , 0   , 6   , 0   , 0   , 0   , 4   , 0  , 0   , 0  , 72  , 0   , 5   , 0   , 18  , 0   , 0   , 0   , 0   , 0   , 0   , 0   ,
    35  , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 71  , 0   , 3   , 0   , 18  , 0   , 0   , 0  , 3   , 0  , 0   , 0   , 71  , 0   , 4   , 0   , 20  , 0   , 0   , 0   , 34  , 0   ,
    0   , 0   , 0   , 0   , 0   , 0   , 71  , 0   , 4   , 0   , 20  , 0   , 0   , 0   , 33  , 0  , 0   , 0  , 0   , 0   , 0   , 0   , 71  , 0   , 4   , 0   , 28  , 0   , 0   , 0   ,
    1   , 0   , 0   , 0   , 1   , 0   , 0   , 0   , 71  , 0   , 4   , 0   , 29  , 0   , 0   , 0  , 1   , 0  , 0   , 0   , 2   , 0   , 0   , 0   , 71  , 0   , 4   , 0   , 30  , 0   ,
    0   , 0   , 1   , 0   , 0   , 0   , 3   , 0   , 0   , 0   , 71  , 0   , 4   , 0   , 31  , 0  , 0   , 0  , 11  , 0   , 0   , 0   , 25  , 0   , 0   , 0   , 19  , 0   , 2   , 0   ,
    2   , 0   , 0   , 0   , 33  , 0   , 3   , 0   , 3   , 0   , 0   , 0   , 2   , 0   , 0   , 0  , 21  , 0  , 4   , 0   , 6   , 0   , 0   , 0   , 32  , 0   , 0   , 0   , 0   , 0   ,
    0   , 0   , 32  , 0   , 4   , 0   , 7   , 0   , 0   , 0   , 7   , 0   , 0   , 0   , 6   , 0  , 0   , 0  , 23  , 0   , 4   , 0   , 9   , 0   , 0   , 0   , 6   , 0   , 0   , 0   ,
    3   , 0   , 0   , 0   , 32  , 0   , 4   , 0   , 10  , 0   , 0   , 0   , 1   , 0   , 0   , 0  , 9   , 0  , 0   , 0   , 59  , 0   , 4   , 0   , 10  , 0   , 0   , 0   , 11  , 0   ,
    0   , 0   , 1   , 0   , 0   , 0   , 43  , 0   , 4   , 0   , 6   , 0   , 0   , 0   , 12  , 0  , 0   , 0  , 0   , 0   , 0   , 0   , 32  , 0   , 4   , 0   , 13  , 0   , 0   , 0   ,
    1   , 0   , 0   , 0   , 6   , 0   , 0   , 0   , 22  , 0   , 3   , 0   , 16  , 0   , 0   , 0  , 32  , 0  , 0   , 0   , 29  , 0   , 3   , 0   , 17  , 0   , 0   , 0   , 16  , 0   ,
    0   , 0   , 30  , 0   , 3   , 0   , 18  , 0   , 0   , 0   , 17  , 0   , 0   , 0   , 32  , 0  , 4   , 0  , 19  , 0   , 0   , 0   , 2   , 0   , 0   , 0   , 18  , 0   , 0   , 0   ,
    59  , 0   , 4   , 0   , 19  , 0   , 0   , 0   , 20  , 0   , 0   , 0   , 2   , 0   , 0   , 0  , 21  , 0  , 4   , 0   , 21  , 0   , 0   , 0   , 32  , 0   , 0   , 0   , 1   , 0   ,
    0   , 0   , 43  , 0   , 4   , 0   , 21  , 0   , 0   , 0   , 22  , 0   , 0   , 0   , 0   , 0  , 0   , 0  , 32  , 0   , 4   , 0   , 26  , 0   , 0   , 0   , 2   , 0   , 0   , 0   ,
    16  , 0   , 0   , 0   , 50  , 0   , 4   , 0   , 6   , 0   , 0   , 0   , 28  , 0   , 0   , 0  , 1   , 0  , 0   , 0   , 50  , 0   , 4   , 0   , 6   , 0   , 0   , 0   , 29  , 0   ,
    0   , 0   , 1   , 0   , 0   , 0   , 50  , 0   , 4   , 0   , 6   , 0   , 0   , 0   , 30  , 0  , 0   , 0  , 1   , 0   , 0   , 0   , 51  , 0   , 6   , 0   , 9   , 0   , 0   , 0   ,
    31  , 0   , 0   , 0   , 28  , 0   , 0   , 0   , 29  , 0   , 0   , 0   , 30  , 0   , 0   , 0  , 54  , 0  , 5   , 0   , 2   , 0   , 0   , 0   , 4   , 0   , 0   , 0   , 0   , 0   ,
    0   , 0   , 3   , 0   , 0   , 0   , 248 , 0   , 2   , 0   , 5   , 0   , 0   , 0   , 59  , 0  , 4   , 0  , 7   , 0   , 0   , 0   , 8   , 0   , 0   , 0   , 7   , 0   , 0   , 0   ,
    65  , 0   , 5   , 0   , 13  , 0   , 0   , 0   , 14  , 0   , 0   , 0   , 11  , 0   , 0   , 0  , 12  , 0  , 0   , 0   , 61  , 0   , 4   , 0   , 6   , 0   , 0   , 0   , 15  , 0   ,
    0   , 0   , 14  , 0   , 0   , 0   , 62  , 0   , 3   , 0   , 8   , 0   , 0   , 0   , 15  , 0  , 0   , 0  , 61  , 0   , 4   , 0   , 6   , 0   , 0   , 0   , 23  , 0   , 0   , 0   ,
    8   , 0   , 0   , 0   , 61  , 0   , 4   , 0   , 6   , 0   , 0   , 0   , 24  , 0   , 0   , 0  , 8   , 0  , 0   , 0   , 112 , 0   , 4   , 0   , 16  , 0   , 0   , 0   , 25  , 0   ,
    0   , 0   , 24  , 0   , 0   , 0   , 65  , 0   , 6   , 0   , 26  , 0   , 0   , 0   , 27  , 0  , 0   , 0  , 20  , 0   , 0   , 0   , 22  , 0   , 0   , 0   , 23  , 0   , 0   , 0   ,
    62  , 0   , 3   , 0   , 27  , 0   , 0   , 0   , 25  , 0   , 0   , 0   , 253 , 0   , 1   , 0  , 56  , 0  , 1   , 0
}};


TEST_CASE("NullProgram", "test_ProgramCreation") {

    auto session = ll::Session::create();
    REQUIRE(session != nullptr);

    REQUIRE_THROWS_AS(session->createProgram("noExist.spv"), std::system_error);
}


TEST_CASE("FromFile", "test_ProgramCreation") {

    auto session = ll::Session::create();
    REQUIRE(session != nullptr);

    auto program = session->createProgram(SHADER_PATH + "/assign.spv");
    REQUIRE(program != nullptr);
}


TEST_CASE("FromSPIRV", "test_ProgramCreation") {

    auto session = ll::Session::create();
    REQUIRE(session != nullptr);

    auto program = session->createProgram(spirv);
    REQUIRE(program != nullptr);
}


TEST_CASE("FromSPIRV_empty", "test_ProgramCreation") {

    auto session = ll::Session::create();
    REQUIRE(session != nullptr);

    std::vector<uint8_t> spirv {};

    REQUIRE_THROWS_AS(session->createProgram(spirv), std::system_error);
}


/**

NOTE: This test does not pass as no exception is thrown
after corrupting the SPIR-V code. The following error message is
displayed when having the validation layer active.

{code}

(null)(ERROR / SPEC): msgNum: 5 - SPIR-V module not valid: Invalid instruction OpSourceExtension starting at word 31: expected no more operands after 7 words, but stated word count is 10.
    Objects: 1
       [0] 0x0, type: 0, name: (null)
Validation(ERROR): msg_code: 5: Object: VK_NULL_HANDLE (Type = 0) | SPIR-V module not valid: Invalid instruction OpSourceExtension starting at word 31: expected no more operands after 7 words, but stated word count is 10.

{encode}
*/
// TEST_CASE("FromSPIRV_corrupted", "test_ProgramCreation") {

//     auto session = ll::Session::create();
//     REQUIRE(session != nullptr);

//     auto spirvCorrupted = spirv;
//     spirvCorrupted[120] = 0;
//     spirvCorrupted[150] = 0;

//     REQUIRE_THROWS_AS(session->createProgram(spirvCorrupted), std::system_error);
// }

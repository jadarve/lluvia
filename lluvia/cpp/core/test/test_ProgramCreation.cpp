/**
 * \file test_SessionCreation.cpp
 * \brief test Session creation
 * \copyright 2017, Juan David Adarve. See AUTHORS for more details
 * \license Apache 2.0, see LICENSE for more details
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "lluvia/core.h"
#include <iostream>

#include "tools/cpp/runfiles/runfiles.h"
using bazel::tools::cpp::runfiles::Runfiles;

/**
SPIR-V for assign.comp shader
*/
auto spirv = std::vector<uint8_t> { {
    0x03,
    0x02,
    0x23,
    0x07,
    0x00,
    0x00,
    0x01,
    0x00,
    0x02,
    0x00,
    0x0D,
    0x00,
    0x20,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x11,
    0x00,
    0x02,
    0x00,
    0x01,
    0x00,
    0x00,
    0x00,
    0x0B,
    0x00,
    0x06,
    0x00,
    0x01,
    0x00,
    0x00,
    0x00,
    0x47,
    0x4C,
    0x53,
    0x4C,
    0x2E,
    0x73,
    0x74,
    0x64,
    0x2E,
    0x34,
    0x35,
    0x30,
    0x00,
    0x00,
    0x00,
    0x00,
    0x0E,
    0x00,
    0x03,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x01,
    0x00,
    0x00,
    0x00,
    0x0F,
    0x00,
    0x06,
    0x00,
    0x05,
    0x00,
    0x00,
    0x00,
    0x04,
    0x00,
    0x00,
    0x00,
    0x6D,
    0x61,
    0x69,
    0x6E,
    0x00,
    0x00,
    0x00,
    0x00,
    0x0B,
    0x00,
    0x00,
    0x00,
    0x10,
    0x00,
    0x06,
    0x00,
    0x04,
    0x00,
    0x00,
    0x00,
    0x11,
    0x00,
    0x00,
    0x00,
    0x01,
    0x00,
    0x00,
    0x00,
    0x01,
    0x00,
    0x00,
    0x00,
    0x01,
    0x00,
    0x00,
    0x00,
    0x03,
    0x00,
    0x03,
    0x00,
    0x02,
    0x00,
    0x00,
    0x00,
    0xC2,
    0x01,
    0x00,
    0x00,
    0x04,
    0x00,
    0x0A,
    0x00,
    0x47,
    0x4C,
    0x5F,
    0x47,
    0x4F,
    0x4F,
    0x47,
    0x4C,
    0x45,
    0x5F,
    0x63,
    0x70,
    0x70,
    0x5F,
    0x73,
    0x74,
    0x79,
    0x6C,
    0x65,
    0x5F,
    0x6C,
    0x69,
    0x6E,
    0x65,
    0x5F,
    0x64,
    0x69,
    0x72,
    0x65,
    0x63,
    0x74,
    0x69,
    0x76,
    0x65,
    0x00,
    0x00,
    0x04,
    0x00,
    0x08,
    0x00,
    0x47,
    0x4C,
    0x5F,
    0x47,
    0x4F,
    0x4F,
    0x47,
    0x4C,
    0x45,
    0x5F,
    0x69,
    0x6E,
    0x63,
    0x6C,
    0x75,
    0x64,
    0x65,
    0x5F,
    0x64,
    0x69,
    0x72,
    0x65,
    0x63,
    0x74,
    0x69,
    0x76,
    0x65,
    0x00,
    0x05,
    0x00,
    0x04,
    0x00,
    0x04,
    0x00,
    0x00,
    0x00,
    0x6D,
    0x61,
    0x69,
    0x6E,
    0x00,
    0x00,
    0x00,
    0x00,
    0x05,
    0x00,
    0x04,
    0x00,
    0x08,
    0x00,
    0x00,
    0x00,
    0x69,
    0x6E,
    0x64,
    0x65,
    0x78,
    0x00,
    0x00,
    0x00,
    0x05,
    0x00,
    0x08,
    0x00,
    0x0B,
    0x00,
    0x00,
    0x00,
    0x67,
    0x6C,
    0x5F,
    0x47,
    0x6C,
    0x6F,
    0x62,
    0x61,
    0x6C,
    0x49,
    0x6E,
    0x76,
    0x6F,
    0x63,
    0x61,
    0x74,
    0x69,
    0x6F,
    0x6E,
    0x49,
    0x44,
    0x00,
    0x00,
    0x00,
    0x05,
    0x00,
    0x04,
    0x00,
    0x12,
    0x00,
    0x00,
    0x00,
    0x6F,
    0x75,
    0x74,
    0x30,
    0x00,
    0x00,
    0x00,
    0x00,
    0x06,
    0x00,
    0x07,
    0x00,
    0x12,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x6F,
    0x75,
    0x74,
    0x70,
    0x75,
    0x74,
    0x42,
    0x75,
    0x66,
    0x66,
    0x65,
    0x72,
    0x00,
    0x00,
    0x00,
    0x00,
    0x05,
    0x00,
    0x03,
    0x00,
    0x14,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x47,
    0x00,
    0x04,
    0x00,
    0x0B,
    0x00,
    0x00,
    0x00,
    0x0B,
    0x00,
    0x00,
    0x00,
    0x1C,
    0x00,
    0x00,
    0x00,
    0x47,
    0x00,
    0x04,
    0x00,
    0x11,
    0x00,
    0x00,
    0x00,
    0x06,
    0x00,
    0x00,
    0x00,
    0x04,
    0x00,
    0x00,
    0x00,
    0x48,
    0x00,
    0x05,
    0x00,
    0x12,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x23,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x47,
    0x00,
    0x03,
    0x00,
    0x12,
    0x00,
    0x00,
    0x00,
    0x03,
    0x00,
    0x00,
    0x00,
    0x47,
    0x00,
    0x04,
    0x00,
    0x14,
    0x00,
    0x00,
    0x00,
    0x22,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x47,
    0x00,
    0x04,
    0x00,
    0x14,
    0x00,
    0x00,
    0x00,
    0x21,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x47,
    0x00,
    0x04,
    0x00,
    0x1C,
    0x00,
    0x00,
    0x00,
    0x01,
    0x00,
    0x00,
    0x00,
    0x01,
    0x00,
    0x00,
    0x00,
    0x47,
    0x00,
    0x04,
    0x00,
    0x1D,
    0x00,
    0x00,
    0x00,
    0x01,
    0x00,
    0x00,
    0x00,
    0x02,
    0x00,
    0x00,
    0x00,
    0x47,
    0x00,
    0x04,
    0x00,
    0x1E,
    0x00,
    0x00,
    0x00,
    0x01,
    0x00,
    0x00,
    0x00,
    0x03,
    0x00,
    0x00,
    0x00,
    0x47,
    0x00,
    0x04,
    0x00,
    0x1F,
    0x00,
    0x00,
    0x00,
    0x0B,
    0x00,
    0x00,
    0x00,
    0x19,
    0x00,
    0x00,
    0x00,
    0x13,
    0x00,
    0x02,
    0x00,
    0x02,
    0x00,
    0x00,
    0x00,
    0x21,
    0x00,
    0x03,
    0x00,
    0x03,
    0x00,
    0x00,
    0x00,
    0x02,
    0x00,
    0x00,
    0x00,
    0x15,
    0x00,
    0x04,
    0x00,
    0x06,
    0x00,
    0x00,
    0x00,
    0x20,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x20,
    0x00,
    0x04,
    0x00,
    0x07,
    0x00,
    0x00,
    0x00,
    0x07,
    0x00,
    0x00,
    0x00,
    0x06,
    0x00,
    0x00,
    0x00,
    0x17,
    0x00,
    0x04,
    0x00,
    0x09,
    0x00,
    0x00,
    0x00,
    0x06,
    0x00,
    0x00,
    0x00,
    0x03,
    0x00,
    0x00,
    0x00,
    0x20,
    0x00,
    0x04,
    0x00,
    0x0A,
    0x00,
    0x00,
    0x00,
    0x01,
    0x00,
    0x00,
    0x00,
    0x09,
    0x00,
    0x00,
    0x00,
    0x3B,
    0x00,
    0x04,
    0x00,
    0x0A,
    0x00,
    0x00,
    0x00,
    0x0B,
    0x00,
    0x00,
    0x00,
    0x01,
    0x00,
    0x00,
    0x00,
    0x2B,
    0x00,
    0x04,
    0x00,
    0x06,
    0x00,
    0x00,
    0x00,
    0x0C,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x20,
    0x00,
    0x04,
    0x00,
    0x0D,
    0x00,
    0x00,
    0x00,
    0x01,
    0x00,
    0x00,
    0x00,
    0x06,
    0x00,
    0x00,
    0x00,
    0x16,
    0x00,
    0x03,
    0x00,
    0x10,
    0x00,
    0x00,
    0x00,
    0x20,
    0x00,
    0x00,
    0x00,
    0x1D,
    0x00,
    0x03,
    0x00,
    0x11,
    0x00,
    0x00,
    0x00,
    0x10,
    0x00,
    0x00,
    0x00,
    0x1E,
    0x00,
    0x03,
    0x00,
    0x12,
    0x00,
    0x00,
    0x00,
    0x11,
    0x00,
    0x00,
    0x00,
    0x20,
    0x00,
    0x04,
    0x00,
    0x13,
    0x00,
    0x00,
    0x00,
    0x02,
    0x00,
    0x00,
    0x00,
    0x12,
    0x00,
    0x00,
    0x00,
    0x3B,
    0x00,
    0x04,
    0x00,
    0x13,
    0x00,
    0x00,
    0x00,
    0x14,
    0x00,
    0x00,
    0x00,
    0x02,
    0x00,
    0x00,
    0x00,
    0x15,
    0x00,
    0x04,
    0x00,
    0x15,
    0x00,
    0x00,
    0x00,
    0x20,
    0x00,
    0x00,
    0x00,
    0x01,
    0x00,
    0x00,
    0x00,
    0x2B,
    0x00,
    0x04,
    0x00,
    0x15,
    0x00,
    0x00,
    0x00,
    0x16,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x20,
    0x00,
    0x04,
    0x00,
    0x1A,
    0x00,
    0x00,
    0x00,
    0x02,
    0x00,
    0x00,
    0x00,
    0x10,
    0x00,
    0x00,
    0x00,
    0x32,
    0x00,
    0x04,
    0x00,
    0x06,
    0x00,
    0x00,
    0x00,
    0x1C,
    0x00,
    0x00,
    0x00,
    0x01,
    0x00,
    0x00,
    0x00,
    0x32,
    0x00,
    0x04,
    0x00,
    0x06,
    0x00,
    0x00,
    0x00,
    0x1D,
    0x00,
    0x00,
    0x00,
    0x01,
    0x00,
    0x00,
    0x00,
    0x32,
    0x00,
    0x04,
    0x00,
    0x06,
    0x00,
    0x00,
    0x00,
    0x1E,
    0x00,
    0x00,
    0x00,
    0x01,
    0x00,
    0x00,
    0x00,
    0x33,
    0x00,
    0x06,
    0x00,
    0x09,
    0x00,
    0x00,
    0x00,
    0x1F,
    0x00,
    0x00,
    0x00,
    0x1C,
    0x00,
    0x00,
    0x00,
    0x1D,
    0x00,
    0x00,
    0x00,
    0x1E,
    0x00,
    0x00,
    0x00,
    0x36,
    0x00,
    0x05,
    0x00,
    0x02,
    0x00,
    0x00,
    0x00,
    0x04,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x03,
    0x00,
    0x00,
    0x00,
    0xF8,
    0x00,
    0x02,
    0x00,
    0x05,
    0x00,
    0x00,
    0x00,
    0x3B,
    0x00,
    0x04,
    0x00,
    0x07,
    0x00,
    0x00,
    0x00,
    0x08,
    0x00,
    0x00,
    0x00,
    0x07,
    0x00,
    0x00,
    0x00,
    0x41,
    0x00,
    0x05,
    0x00,
    0x0D,
    0x00,
    0x00,
    0x00,
    0x0E,
    0x00,
    0x00,
    0x00,
    0x0B,
    0x00,
    0x00,
    0x00,
    0x0C,
    0x00,
    0x00,
    0x00,
    0x3D,
    0x00,
    0x04,
    0x00,
    0x06,
    0x00,
    0x00,
    0x00,
    0x0F,
    0x00,
    0x00,
    0x00,
    0x0E,
    0x00,
    0x00,
    0x00,
    0x3E,
    0x00,
    0x03,
    0x00,
    0x08,
    0x00,
    0x00,
    0x00,
    0x0F,
    0x00,
    0x00,
    0x00,
    0x3D,
    0x00,
    0x04,
    0x00,
    0x06,
    0x00,
    0x00,
    0x00,
    0x17,
    0x00,
    0x00,
    0x00,
    0x08,
    0x00,
    0x00,
    0x00,
    0x3D,
    0x00,
    0x04,
    0x00,
    0x06,
    0x00,
    0x00,
    0x00,
    0x18,
    0x00,
    0x00,
    0x00,
    0x08,
    0x00,
    0x00,
    0x00,
    0x70,
    0x00,
    0x04,
    0x00,
    0x10,
    0x00,
    0x00,
    0x00,
    0x19,
    0x00,
    0x00,
    0x00,
    0x18,
    0x00,
    0x00,
    0x00,
    0x41,
    0x00,
    0x06,
    0x00,
    0x1A,
    0x00,
    0x00,
    0x00,
    0x1B,
    0x00,
    0x00,
    0x00,
    0x14,
    0x00,
    0x00,
    0x00,
    0x16,
    0x00,
    0x00,
    0x00,
    0x17,
    0x00,
    0x00,
    0x00,
    0x3E,
    0x00,
    0x03,
    0x00,
    0x1B,
    0x00,
    0x00,
    0x00,
    0x19,
    0x00,
    0x00,
    0x00,
    0xFD,
    0x00,
    0x01,
    0x00,
    0x38,
    0x00,
    0x01,
    0x00,
} };

TEST_CASE("NullProgram", "test_ProgramCreation")
{

    auto session = ll::Session::create(ll::SessionDescriptor().enableDebug(true));
    REQUIRE(session != nullptr);

    REQUIRE_THROWS_AS(session->createProgram("noExist.spv"), std::system_error);
}

TEST_CASE("FromFile", "test_ProgramCreation")
{

    auto runfiles = Runfiles::CreateForTest(nullptr);
    REQUIRE(runfiles != nullptr);

    auto session = ll::Session::create(ll::SessionDescriptor().enableDebug(true));
    REQUIRE(session != nullptr);

    auto program = std::shared_ptr<ll::Program> { nullptr };
    REQUIRE_NOTHROW([&]() {
        auto path = runfiles->Rlocation("lluvia/lluvia/cpp/core/test/glsl/assign.comp.spv");
        program = session->createProgram(path);
    }());
    REQUIRE(program != nullptr);

    REQUIRE_FALSE(session->hasReceivedVulkanWarningMessages());
}

TEST_CASE("FromSPIRV", "test_ProgramCreation")
{

    auto session = ll::Session::create(ll::SessionDescriptor().enableDebug(true));
    REQUIRE(session != nullptr);

    auto program = session->createProgram(spirv);
    REQUIRE(program != nullptr);

    REQUIRE_FALSE(session->hasReceivedVulkanWarningMessages());
}

TEST_CASE("FromSPIRV_empty", "test_ProgramCreation")
{

    auto session = ll::Session::create(ll::SessionDescriptor().enableDebug(true));
    REQUIRE(session != nullptr);

    std::vector<uint8_t> emptySpirv {};

    REQUIRE_THROWS_AS(session->createProgram(emptySpirv), std::system_error);

    REQUIRE_FALSE(session->hasReceivedVulkanWarningMessages());
}

TEST_CASE("SetInProgramRegistry", "test_ProgramCreation")
{

    auto error = std::string {};
    auto runfiles = Runfiles::CreateForTest(&error);
    REQUIRE(runfiles != nullptr);

    auto session = ll::Session::create(ll::SessionDescriptor().enableDebug(true));
    REQUIRE(session != nullptr);

    auto program = std::shared_ptr<ll::Program> { nullptr };
    REQUIRE_NOTHROW([&]() {
        auto path = runfiles->Rlocation("lluvia/lluvia/cpp/core/test/glsl/assign.comp.spv");
        program = session->createProgram(path);
    }());
    REQUIRE(program != nullptr);

    session->setProgram("assign", program);
    // session and program should be destroyed gracefully.

    REQUIRE_FALSE(session->hasReceivedVulkanWarningMessages());
}

/**

NOTE: This test expects to receive a warning message like this:

{code}

(null)(ERROR / SPEC): msgNum: 5 - SPIR-V module not valid: Invalid instruction
    OpSourceExtension starting at word 31: expected no more operands after 7 words,
    but stated word count is 10.
    Objects: 1
       [0] 0x0, type: 0, name: (null)
Validation(ERROR): msg_code: 5: Object: VK_NULL_HANDLE (Type = 0) | SPIR-V module not valid:
    Invalid instruction OpSourceExtension starting at word 31: expected no more operands
    after 7 words, but stated word count is 10.

{encode}
*/
TEST_CASE("FromSPIRV_corrupted", "test_ProgramCreation")
{

    auto session = ll::Session::create(ll::SessionDescriptor().enableDebug(true));
    REQUIRE(session != nullptr);

    auto spirvCorrupted = spirv;
    spirvCorrupted[120] = 0;
    spirvCorrupted[150] = 0;

    REQUIRE_NOTHROW(session->createProgram(spirvCorrupted));

    // I expect to receive a warning message
    REQUIRE(session->hasReceivedVulkanWarningMessages());
}

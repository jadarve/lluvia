/**
 * \file HSVA2RGBA_test.cpp
 * \brief Test HSVA2RGBA node.
 * \copyright 2021, Juan David Adarve. See AUTHORS for more details
 * \license Apache 2.0, see LICENSE for more details
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include <cmath>
#include <memory>
#include "lluvia/core.h"
#include "lluvia/cpp/core/_virtual_includes/core_cc_library/lluvia/core.h"

#include "tools/cpp/runfiles/runfiles.h"
using bazel::tools::cpp::runfiles::Runfiles;

TEST_CASE("goodUse", "HSVA2RGBA_test") {

    auto runfiles = Runfiles::CreateForTest(nullptr);
    REQUIRE(runfiles != nullptr);

    ///////////////////////////////////////////////////////
    // Create a session and a memory
    ///////////////////////////////////////////////////////
    auto session = ll::Session::create(ll::SessionDescriptor().enableDebug(true));
    REQUIRE(session != nullptr);

    auto memory = session->createMemory(ll::MemoryPropertyFlagBits::DeviceLocal, 0, false);
    REQUIRE(memory != nullptr);

    ///////////////////////////////////////////////////////
    // Register program and builder
    ///////////////////////////////////////////////////////
    auto program = session->createProgram(runfiles->Rlocation("lluvia/lluvia/nodes/lluvia/color/HSVA2RGBA.spv"));
    REQUIRE(program != nullptr);
    session->setProgram("lluvia/color/HSVA2RGBA", program);

    REQUIRE_NOTHROW(session->scriptFile(runfiles->Rlocation("lluvia/lluvia/nodes/lluvia/color/HSVA2RGBA.lua")));

    ///////////////////////////////////////////////////////
    // Create the inputs
    ///////////////////////////////////////////////////////
    auto in_hsva_imgDesc = ll::ImageDescriptor{}
            .setWidth(640)
            .setHeight(480)
            .setChannelType(ll::ChannelType::Float32)
            .setChannelCount(ll::ChannelCount::C4)
            .setUsageFlags(vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst);

    auto in_hsva_viewDesc = ll::ImageViewDescriptor {}
            .setFilterMode(ll::ImageFilterMode::Nearest)
            .setAddressMode(ll::ImageAddressMode::ClampToEdge)
            .setNormalizedCoordinates(false);

    auto in_hsva = ll::createAndInitImageView(session, memory, in_hsva_imgDesc, in_hsva_viewDesc);
    REQUIRE(in_hsva != nullptr);

    ///////////////////////////////////////////////////////
    // Create the node
    ///////////////////////////////////////////////////////
    auto node = std::shared_ptr<ll::ComputeNode> {nullptr};

    REQUIRE_NOTHROW(([&]{
        node = session->createComputeNode("lluvia/color/HSVA2RGBA");
        })());

    REQUIRE_NOTHROW(node->bind("in_hsva", in_hsva));
    REQUIRE_NOTHROW(node->init());

    ///////////////////////////////////////////////////////
    // Validate out_rgba
    ///////////////////////////////////////////////////////
    auto out_rgba_object = std::shared_ptr<ll::Object> {nullptr};
    REQUIRE_NOTHROW(([&]{
        out_rgba_object = node->getPort("out_rgba");
        })());

    REQUIRE(out_rgba_object != nullptr);
    REQUIRE(out_rgba_object->getType() == ll::ObjectType::ImageView);

    auto out_rgba = std::static_pointer_cast<ll::ImageView>(out_rgba_object);
    REQUIRE(out_rgba->getWidth() == in_hsva->getWidth());
    REQUIRE(out_rgba->getHeight() == in_hsva->getHeight());
    REQUIRE(out_rgba->getDepth() == in_hsva->getDepth());

    REQUIRE(out_rgba->getChannelType() == ll::ChannelType::Uint8);
    REQUIRE(out_rgba->getChannelCount() == ll::ChannelCount::C4);

    ///////////////////////////////////////////////////////
    // Run the node
    ///////////////////////////////////////////////////////
    session->run(*node);

    ///////////////////////////////////////////////////////
    // Validate good usage of Vulkan :)
    ///////////////////////////////////////////////////////
    REQUIRE_FALSE(ll::hasReceivedVulkanWarningMessages());
}

/**
 * \file RGBA2Gray_test.cpp
 * \brief Test RGBA2Gray node.
 * \copyright 2021, Juan David Adarve. See AUTHORS for more details
 * \license Apache 2.0, see LICENSE for more details
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include <cmath>
#include <memory>
#include "lluvia/core.h"

#include "tools/cpp/runfiles/runfiles.h"
using bazel::tools::cpp::runfiles::Runfiles;

TEST_CASE("goodUse", "RGBA2Gray_test") {

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
    auto program = session->createProgram(runfiles->Rlocation("lluvia/lluvia/nodes/lluvia/color/RGBA2Gray.comp.spv"));
    REQUIRE(program != nullptr);
    session->setProgram("lluvia/color/RGBA2Gray.comp", program);

    REQUIRE_NOTHROW(session->scriptFile(runfiles->Rlocation("lluvia/lluvia/nodes/lluvia/color/RGBA2Gray.lua")));

    ///////////////////////////////////////////////////////
    // Create the inputs
    ///////////////////////////////////////////////////////
    auto in_rgba_imgDesc = ll::ImageDescriptor{}
            .setWidth(640)
            .setHeight(480)
            .setChannelType(ll::ChannelType::Uint8)
            .setChannelCount(ll::ChannelCount::C4)
            .setUsageFlags(ll::ImageUsageFlagBits::Storage | ll::ImageUsageFlagBits::Sampled | ll::ImageUsageFlagBits::TransferDst);

    auto in_rgba_viewDesc = ll::ImageViewDescriptor {}
            .setFilterMode(ll::ImageFilterMode::Nearest)
            .setAddressMode(ll::ImageAddressMode::ClampToEdge)
            .setNormalizedCoordinates(false);

    auto in_rgba = ll::createAndInitImageView(session, memory, in_rgba_imgDesc, in_rgba_viewDesc);
    REQUIRE(in_rgba != nullptr);

    ///////////////////////////////////////////////////////
    // Create the node
    ///////////////////////////////////////////////////////
    auto node = std::shared_ptr<ll::ComputeNode> {nullptr};

    REQUIRE_NOTHROW(([&]{
        node = session->createComputeNode("lluvia/color/RGBA2Gray");
        })());

    REQUIRE_NOTHROW(node->bind("in_rgba", in_rgba));
    REQUIRE_NOTHROW(node->init());

    ///////////////////////////////////////////////////////
    // Validate out_gray
    ///////////////////////////////////////////////////////
    auto out_gray_object = std::shared_ptr<ll::Object> {nullptr};
    REQUIRE_NOTHROW(([&]{
        out_gray_object = node->getPort("out_gray");
    })());

    REQUIRE(out_gray_object != nullptr);
    REQUIRE(out_gray_object->getType() == ll::ObjectType::ImageView);

    auto out_gray = std::static_pointer_cast<ll::ImageView>(out_gray_object);
    REQUIRE(out_gray->getWidth() == in_rgba->getWidth());
    REQUIRE(out_gray->getHeight() == in_rgba->getHeight());
    REQUIRE(out_gray->getDepth() == in_rgba->getDepth());

    REQUIRE(out_gray->getChannelType() == ll::ChannelType::Uint8);
    REQUIRE(out_gray->getChannelCount() == ll::ChannelCount::C1);

    ///////////////////////////////////////////////////////
    // Run the node
    ///////////////////////////////////////////////////////
    session->run(*node);

    ///////////////////////////////////////////////////////
    // Validate good usage of Vulkan :)
    ///////////////////////////////////////////////////////
    REQUIRE_FALSE(session->hasReceivedVulkanWarningMessages());
}

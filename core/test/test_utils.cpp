/**
 * \file test_utils.cpp
 * \brief test utility functions
 * \copyright 2017, Juan David Adarve. See AUTHORS for more details
 * \license Apache 2.0, see LICENSE for more details
 */

#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"

#include "lluvia/core.h"

using memflags = vk::MemoryPropertyFlagBits;


std::shared_ptr<ll::Image> createInitImage(
    std::shared_ptr<ll::Session>& session,
    std::shared_ptr<ll::Memory>& memory,
    const ll::ImageDescriptor& desc,
    const vk::ImageLayout initialLayout) {
    
    std::shared_ptr<ll::Image> image = memory->createImage(desc);

    {
        auto cmdBuffer = session->createCommandBuffer();

        cmdBuffer->begin();
        cmdBuffer->changeImageLayout(*image, vk::ImageLayout::eGeneral);
        cmdBuffer->end();

        session->run(*cmdBuffer);
    }

    return image;
}

struct binding {

    binding(const uint32_t index, const std::shared_ptr<ll::Object>& object) :
        index  {index},
        object {object} {

    }

    const uint32_t index;
    const std::shared_ptr<ll::Object>& object;
};

void configureComputeNode(std::shared_ptr<ll::ComputeNode>& node,
    const ll::vec3ui&& globalSize,
    std::initializer_list<std::pair<uint32_t, const std::shared_ptr<ll::Object>&> > bindings
    ) {

}


TEST_CASE("createInitImage", "test_utils") {

    // Constants
    const auto memoryFlags = memflags::eDeviceLocal;

    const auto width = uint32_t {1080};
    const auto height = uint32_t {1920};

    const vk::ImageUsageFlags imgUsageFlags = { vk::ImageUsageFlagBits::eStorage
                                              | vk::ImageUsageFlagBits::eSampled
                                              | vk::ImageUsageFlagBits::eTransferDst};

    const auto imgDesc = ll::ImageDescriptor {width, height, 1, 1, ll::ChannelType::Uint8, imgUsageFlags};


    auto session = ll::Session::create();
    auto memory = session->createMemory(memoryFlags, 0);

    // could create several images at the same time
    auto image = createInitImage(session, memory, imgDesc, vk::ImageLayout::eGeneral);
}


TEST_CASE("configureGraph", "test_utils") {

    // Constants
    const auto memoryFlags = memflags::eDeviceLocal;

    const auto width = uint32_t {1080};
    const auto height = uint32_t {1920};

    const vk::ImageUsageFlags imgUsageFlags = { vk::ImageUsageFlagBits::eStorage
                                              | vk::ImageUsageFlagBits::eSampled
                                              | vk::ImageUsageFlagBits::eTransferDst};

    const auto imgDesc = ll::ImageDescriptor {width, height, 1, 1, ll::ChannelType::Uint8, imgUsageFlags};

    auto session = ll::Session::create();
    auto memory = session->createMemory(memoryFlags, 0);

    const auto RGBADesc = ll::ImageDescriptor(imgDesc).setChannelCount(4);
    const auto grayDesc = ll::ImageDescriptor(imgDesc).setChannelCount(1);

    // TOTHINK: Could initialiaze both images with a single command buffer. More efficient.
    auto RGBA = createInitImage(session, memory, RGBADesc, vk::ImageLayout::eGeneral);
    auto gray = createInitImage(session, memory, grayDesc, vk::ImageLayout::eGeneral);

    auto rgba2GrayNode = session->readComputeNode("/home/jadarve/git/lluvia/local/nodes/RGBA2Gray.json");
    configureComputeNode(rgba2GrayNode,
                    RGBA->getShape(),
                    {{0, RGBA}, {1, gray}} // grid
                      );

    // {RGBA->getWidth(), RGBA->getHeight(), RGBA->getDepth()}
}

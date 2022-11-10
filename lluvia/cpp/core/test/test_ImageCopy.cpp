/**
 * \file test_ImageCopy.cpp
 * \brief test image copy operations
 * \copyright 2017, Juan David Adarve. See AUTHORS for more details
 * \license Apache 2.0, see LICENSE for more details
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "lluvia/core.h"
#include <iostream>

using memflags = ll::MemoryPropertyFlagBits;

TEST_CASE("ImageToImage", "ImageCopyTest")
{

    constexpr const auto width = 128;
    constexpr const auto height = 128;
    constexpr const auto pageSize = 32 * 1024 * 1024;
    const ll::ImageUsageFlags imgUsageFlags = {
        ll::ImageUsageFlagBits::Storage
        | ll::ImageUsageFlagBits::Sampled
        | ll::ImageUsageFlagBits::TransferSrc
        | ll::ImageUsageFlagBits::TransferDst
    };

    const auto deviceMemFlags = memflags::DeviceLocal;
    const auto hostMemFlags = memflags::HostCoherent | memflags::HostVisible;

    auto session = ll::Session::create(ll::SessionDescriptor().enableDebug(true));
    REQUIRE(session != nullptr);

    auto deviceMemory = session->createMemory(deviceMemFlags, pageSize);
    REQUIRE(deviceMemory != nullptr);

    auto hostMemory = session->createMemory(hostMemFlags, 0);
    REQUIRE(hostMemory != nullptr);

    auto desc = ll::ImageDescriptor {}
                    .setWidth(width)
                    .setHeight(height)
                    .setDepth(1)
                    .setChannelCount(ll::ChannelCount::C1)
                    .setChannelType(ll::ChannelType::Uint8)
                    .setUsageFlags(imgUsageFlags);

    auto src = deviceMemory->createImage(desc);
    REQUIRE(src != nullptr);

    auto dst = deviceMemory->createImage(desc);
    REQUIRE(dst != nullptr);

    auto srcBuffer = hostMemory->createBuffer(width * height * static_cast<uint64_t>(src->getChannelCount()) * src->getChannelTypeSize());
    REQUIRE(srcBuffer != nullptr);

    {
        auto ptr = srcBuffer->map<uint8_t[]>();

        for (auto i = 0u; i < srcBuffer->getSize(); ++i) {
            ptr[i] = i % 255;
        }
    }

    auto copyCmdBuffer = session->createCommandBuffer();
    REQUIRE(copyCmdBuffer != nullptr);

    copyCmdBuffer->begin();
    copyCmdBuffer->changeImageLayout(*src, ll::ImageLayout::TransferDstOptimal);
    copyCmdBuffer->copyBufferToImage(*srcBuffer, *src);
    copyCmdBuffer->changeImageLayout(*src, ll::ImageLayout::General);
    copyCmdBuffer->end();

    session->run(*copyCmdBuffer);

    auto cmdBuffer = session->createCommandBuffer();
    REQUIRE(cmdBuffer != nullptr);

    cmdBuffer->begin();
    cmdBuffer->changeImageLayout(*src, ll::ImageLayout::TransferSrcOptimal);
    cmdBuffer->changeImageLayout(*dst, ll::ImageLayout::TransferDstOptimal);
    cmdBuffer->copyImageToImage(*src, *dst);
    cmdBuffer->end();

    session->run(*cmdBuffer);

    auto dstBuffer = hostMemory->createBuffer(width * height * static_cast<uint64_t>(dst->getChannelCount()) * dst->getChannelTypeSize());
    REQUIRE(dstBuffer != nullptr);

    auto dstCopyCmdBuffer = session->createCommandBuffer();
    REQUIRE(dstCopyCmdBuffer != nullptr);
    dstCopyCmdBuffer->begin();
    dstCopyCmdBuffer->changeImageLayout(*dst, ll::ImageLayout::TransferSrcOptimal);
    dstCopyCmdBuffer->copyImageToBuffer(*dst, *dstBuffer);
    dstCopyCmdBuffer->changeImageLayout(*dst, ll::ImageLayout::General);
    dstCopyCmdBuffer->end();

    session->run(*dstCopyCmdBuffer);

    {
        auto srcPtr = srcBuffer->map<uint8_t[]>();
        auto dstPtr = dstBuffer->map<uint8_t[]>();

        for (auto i = 0u; i < srcBuffer->getSize(); ++i) {
            REQUIRE(srcPtr[i] == dstPtr[i]);
        }
    }

    REQUIRE_FALSE(session->hasReceivedVulkanWarningMessages());
}

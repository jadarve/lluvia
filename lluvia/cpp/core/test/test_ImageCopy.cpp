/**
 * \file test_ImageCopy.cpp
 * \brief test image copy operations
 * \copyright 2017, Juan David Adarve. See AUTHORS for more details
 * \license Apache 2.0, see LICENSE for more details
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include <iostream>
#include "lluvia/core.h"

using memflags = vk::MemoryPropertyFlagBits;

TEST_CASE("ImageToImage", "ImageCopyTest") {

    constexpr const auto width  = 128;
    constexpr const auto height = 128;
    constexpr const auto pageSize = 32 * 1024 * 1024;
    const vk::ImageUsageFlags imgUsageFlags = {
          vk::ImageUsageFlagBits::eStorage
        | vk::ImageUsageFlagBits::eSampled
        | vk::ImageUsageFlagBits::eTransferSrc
        | vk::ImageUsageFlagBits::eTransferDst};

    const auto deviceMemFlags = memflags::eDeviceLocal;
    const auto hostMemFlags = memflags::eHostCoherent | memflags::eHostVisible;

    auto session = ll::Session::create();
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

    auto srcBuffer = hostMemory->createBuffer(src->getSize());
    REQUIRE(srcBuffer != nullptr);

    {
        auto ptr = srcBuffer->map<uint8_t[]>();

        for(auto i = 0u; i < src->getSize(); ++i) {
            ptr[i] = i % 255;
        }
    }

    auto copyCmdBuffer = session->createCommandBuffer();
    REQUIRE(copyCmdBuffer != nullptr);

    copyCmdBuffer->begin();
    copyCmdBuffer->changeImageLayout(*src, vk::ImageLayout::eTransferDstOptimal);
    copyCmdBuffer->copyBufferToImage(*srcBuffer, *src);
    copyCmdBuffer->changeImageLayout(*src, vk::ImageLayout::eGeneral);
    copyCmdBuffer->end();

    session->run(*copyCmdBuffer);

    auto cmdBuffer = session->createCommandBuffer();
    REQUIRE(cmdBuffer != nullptr);

    cmdBuffer->begin();
    cmdBuffer->changeImageLayout(*src, vk::ImageLayout::eTransferSrcOptimal);
    cmdBuffer->changeImageLayout(*dst, vk::ImageLayout::eTransferDstOptimal);
    cmdBuffer->copyImageToImage(*src, *dst);
    cmdBuffer->end();

    session->run(*cmdBuffer);

    auto dstBuffer = hostMemory->createBuffer(width * height * static_cast<uint64_t>(dst->getChannelCount()) * dst->getChannelTypeSize());
    REQUIRE(dstBuffer != nullptr);

    auto dstCopyCmdBuffer = session->createCommandBuffer();
    REQUIRE(dstCopyCmdBuffer != nullptr);
    dstCopyCmdBuffer->begin();
    dstCopyCmdBuffer->changeImageLayout(*dst, vk::ImageLayout::eTransferSrcOptimal);
    dstCopyCmdBuffer->copyImageToBuffer(*dst, *dstBuffer);
    dstCopyCmdBuffer->changeImageLayout(*dst, vk::ImageLayout::eGeneral);
    dstCopyCmdBuffer->end();

    session->run(*dstCopyCmdBuffer);

    {
        auto srcPtr = srcBuffer->map<uint8_t[]>();
        auto dstPtr = dstBuffer->map<uint8_t[]>();

        for(auto i = 0u; i < srcBuffer->getSize(); ++i) {
            REQUIRE(srcPtr[i] == dstPtr[i]);
        }
    }

}

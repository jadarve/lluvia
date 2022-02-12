/**
 * \file test_PortDescriptor.cpp
 * \brief test port descriptors
 * \copyright 2022, Juan David Adarve. See AUTHORS for more details
 * \license Apache 2.0, see LICENSE for more details
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include <iostream>
#include "lluvia/core.h"

TEST_CASE("CheckNull", "test_PortDescriptor") {

    auto portDescriptor = ll::PortDescriptor{0, "in_buffer", ll::PortDirection::In, ll::PortType::Buffer};

    auto result = portDescriptor.isValid(nullptr);
    REQUIRE(result.first == false);
    REQUIRE(result.second == "null port");
}

TEST_CASE("CheckBuffer", "test_PortDescriptor") {

    auto session = ll::Session::create(ll::SessionDescriptor().enableDebug(true));
    REQUIRE(session != nullptr);

    auto memory = session->createMemory(ll::MemoryPropertyFlagBits::DeviceLocal, 0, false);
    REQUIRE(memory != nullptr);

    auto buffer = memory->createBuffer(1024);
    REQUIRE(buffer != nullptr);

    auto portDescriptor = ll::PortDescriptor{0, "in_buffer", ll::PortDirection::In, ll::PortType::Buffer};

    auto result = portDescriptor.isValid(buffer);
    REQUIRE(result.first == true);
    REQUIRE(result.second == "");
}

TEST_CASE("CheckBufferIncorrectPortType", "test_PortDescriptor") {

    auto session = ll::Session::create(ll::SessionDescriptor().enableDebug(true));
    REQUIRE(session != nullptr);

    auto memory = session->createMemory(ll::MemoryPropertyFlagBits::DeviceLocal, 0, false);
    REQUIRE(memory != nullptr);

    auto buffer = memory->createBuffer(1024);
    REQUIRE(buffer != nullptr);

    auto portDescriptor = ll::PortDescriptor{0, "in_img_view", ll::PortDirection::In, ll::PortType::ImageView};

    auto result = portDescriptor.isValid(buffer);
    REQUIRE(result.first == false);
    REQUIRE(result.second == "Port {binding: 0, name: in_img_view, portType: ll::PortType::ImageView} cannot receive object of type ll::PortType::Buffer");
}

TEST_CASE("CheckImageView", "test_PortDescriptor") {

    auto session = ll::Session::create(ll::SessionDescriptor().enableDebug(true));
    REQUIRE(session != nullptr);

    auto memory = session->createMemory(ll::MemoryPropertyFlagBits::DeviceLocal, 0, false);
    REQUIRE(memory != nullptr);

    const ll::ImageUsageFlags imgUsageFlags = {ll::ImageUsageFlagBits::Storage | ll::ImageUsageFlagBits::Sampled | ll::ImageUsageFlagBits::TransferDst};

    auto desc = ll::ImageDescriptor{}
                    .setWidth(640)
                    .setHeight(480)
                    .setChannelType(ll::ChannelType::Uint8)
                    .setChannelCount(ll::ChannelCount::C1)
                    .setUsageFlags(imgUsageFlags);

    auto imgViewDesc = ll::ImageViewDescriptor{}
                           .setFilterMode(ll::ImageFilterMode::Nearest)
                           .setAddressMode(ll::ImageAddressMode::ClampToEdge)
                           .setNormalizedCoordinates(false)
                           .setIsSampled(false);

    auto imageView = ll::createAndInitImageView(session, memory, desc, imgViewDesc, ll::ImageLayout::General);
    REQUIRE(imageView != nullptr);

    auto portDescriptor = ll::PortDescriptor{0, "in_img_view", ll::PortDirection::In, ll::PortType::ImageView};

    auto result = portDescriptor.isValid(imageView);
    REQUIRE(result.first == true);
    REQUIRE(result.second == "");

    REQUIRE_FALSE(session->hasReceivedVulkanWarningMessages());
}

TEST_CASE("CheckImageViewIncorrectPortType", "test_PortDescriptor") {

    auto session = ll::Session::create(ll::SessionDescriptor().enableDebug(true));
    REQUIRE(session != nullptr);

    auto memory = session->createMemory(ll::MemoryPropertyFlagBits::DeviceLocal, 0, false);
    REQUIRE(memory != nullptr);

    const ll::ImageUsageFlags imgUsageFlags = {ll::ImageUsageFlagBits::Storage | ll::ImageUsageFlagBits::Sampled | ll::ImageUsageFlagBits::TransferDst};

    auto desc = ll::ImageDescriptor{}
                    .setWidth(640)
                    .setHeight(480)
                    .setChannelType(ll::ChannelType::Uint8)
                    .setChannelCount(ll::ChannelCount::C1)
                    .setUsageFlags(imgUsageFlags);

    auto imgViewDesc = ll::ImageViewDescriptor{}
                           .setFilterMode(ll::ImageFilterMode::Nearest)
                           .setAddressMode(ll::ImageAddressMode::ClampToEdge)
                           .setNormalizedCoordinates(false)
                           .setIsSampled(false);

    auto imageView = ll::createAndInitImageView(session, memory, desc, imgViewDesc, ll::ImageLayout::General);
    REQUIRE(imageView != nullptr);

    auto portDescriptor = ll::PortDescriptor{0, "in_buffer", ll::PortDirection::In, ll::PortType::Buffer};

    auto result = portDescriptor.isValid(imageView);
    REQUIRE(result.first == false);
    REQUIRE(result.second == "Port {binding: 0, name: in_buffer, portType: ll::PortType::Buffer} cannot receive object of type ll::PortType::ImageView");

    REQUIRE_FALSE(session->hasReceivedVulkanWarningMessages());
}

TEST_CASE("CheckImageViewChannelCount_positive", "test_PortDescriptor") {

    auto session = ll::Session::create(ll::SessionDescriptor().enableDebug(true));
    REQUIRE(session != nullptr);

    auto memory = session->createMemory(ll::MemoryPropertyFlagBits::DeviceLocal, 0, false);
    REQUIRE(memory != nullptr);

    const ll::ImageUsageFlags imgUsageFlags = {ll::ImageUsageFlagBits::Storage | ll::ImageUsageFlagBits::Sampled | ll::ImageUsageFlagBits::TransferDst};

    auto desc = ll::ImageDescriptor{}
                    .setWidth(640)
                    .setHeight(480)
                    .setChannelType(ll::ChannelType::Uint8)
                    .setChannelCount(ll::ChannelCount::C4)
                    .setUsageFlags(imgUsageFlags);

    auto imgViewDesc = ll::ImageViewDescriptor{}
                           .setFilterMode(ll::ImageFilterMode::Nearest)
                           .setAddressMode(ll::ImageAddressMode::ClampToEdge)
                           .setNormalizedCoordinates(false)
                           .setIsSampled(false);

    auto imageView = ll::createAndInitImageView(session, memory, desc, imgViewDesc, ll::ImageLayout::General);
    REQUIRE(imageView != nullptr);

    auto portDescriptor = ll::PortDescriptor{0, "in_img_view", ll::PortDirection::In, ll::PortType::ImageView};
    portDescriptor.checkImageChannelCountIs(ll::ChannelCount::C4);

    auto result = portDescriptor.isValid(imageView);
    REQUIRE(result.first == true);
    REQUIRE(result.second == "");

    REQUIRE_FALSE(session->hasReceivedVulkanWarningMessages());
}

TEST_CASE("CheckImageViewChannelCount_negative", "test_PortDescriptor")
{

    auto session = ll::Session::create(ll::SessionDescriptor().enableDebug(true));
    REQUIRE(session != nullptr);

    auto memory = session->createMemory(ll::MemoryPropertyFlagBits::DeviceLocal, 0, false);
    REQUIRE(memory != nullptr);

    const ll::ImageUsageFlags imgUsageFlags = {ll::ImageUsageFlagBits::Storage | ll::ImageUsageFlagBits::Sampled | ll::ImageUsageFlagBits::TransferDst};

    auto desc = ll::ImageDescriptor{}
                    .setWidth(640)
                    .setHeight(480)
                    .setChannelType(ll::ChannelType::Uint8)
                    .setChannelCount(ll::ChannelCount::C4)
                    .setUsageFlags(imgUsageFlags);

    auto imgViewDesc = ll::ImageViewDescriptor{}
                           .setFilterMode(ll::ImageFilterMode::Nearest)
                           .setAddressMode(ll::ImageAddressMode::ClampToEdge)
                           .setNormalizedCoordinates(false)
                           .setIsSampled(false);

    auto imageView = ll::createAndInitImageView(session, memory, desc, imgViewDesc, ll::ImageLayout::General);
    REQUIRE(imageView != nullptr);

    auto portDescriptor = ll::PortDescriptor{0, "in_img_view", ll::PortDirection::In, ll::PortType::ImageView};
    portDescriptor.checkImageChannelCountIs(ll::ChannelCount::C1);

    auto result = portDescriptor.isValid(imageView);
    REQUIRE(result.first == false);
    REQUIRE(result.second == "Port {binding: 0, name: in_img_view, portType: ll::PortType::ImageView} invalid image channel count, expecting: 1 got: 4");

    REQUIRE_FALSE(session->hasReceivedVulkanWarningMessages());
}

TEST_CASE("CheckImageViewChannelType_positive", "test_PortDescriptor") {

    auto session = ll::Session::create(ll::SessionDescriptor().enableDebug(true));
    REQUIRE(session != nullptr);

    auto memory = session->createMemory(ll::MemoryPropertyFlagBits::DeviceLocal, 0, false);
    REQUIRE(memory != nullptr);

    const ll::ImageUsageFlags imgUsageFlags = {ll::ImageUsageFlagBits::Storage | ll::ImageUsageFlagBits::Sampled | ll::ImageUsageFlagBits::TransferDst};

    auto desc = ll::ImageDescriptor{}
                    .setWidth(640)
                    .setHeight(480)
                    .setChannelType(ll::ChannelType::Uint8)
                    .setChannelCount(ll::ChannelCount::C4)
                    .setUsageFlags(imgUsageFlags);

    auto imgViewDesc = ll::ImageViewDescriptor{}
                           .setFilterMode(ll::ImageFilterMode::Nearest)
                           .setAddressMode(ll::ImageAddressMode::ClampToEdge)
                           .setNormalizedCoordinates(false)
                           .setIsSampled(false);

    auto imageView = ll::createAndInitImageView(session, memory, desc, imgViewDesc, ll::ImageLayout::General);
    REQUIRE(imageView != nullptr);

    auto portDescriptor = ll::PortDescriptor{0, "in_img_view", ll::PortDirection::In, ll::PortType::ImageView};
    portDescriptor.checkImageChannelTypeIs(ll::ChannelType::Uint8);

    auto result = portDescriptor.isValid(imageView);
    REQUIRE(result.first == true);
    REQUIRE(result.second == "");

    REQUIRE_FALSE(session->hasReceivedVulkanWarningMessages());
}

TEST_CASE("CheckImageViewChannelType_negative", "test_PortDescriptor") {

    auto session = ll::Session::create(ll::SessionDescriptor().enableDebug(true));
    REQUIRE(session != nullptr);

    auto memory = session->createMemory(ll::MemoryPropertyFlagBits::DeviceLocal, 0, false);
    REQUIRE(memory != nullptr);

    const ll::ImageUsageFlags imgUsageFlags = {ll::ImageUsageFlagBits::Storage | ll::ImageUsageFlagBits::Sampled | ll::ImageUsageFlagBits::TransferDst};

    auto desc = ll::ImageDescriptor{}
                    .setWidth(640)
                    .setHeight(480)
                    .setChannelType(ll::ChannelType::Uint8)
                    .setChannelCount(ll::ChannelCount::C4)
                    .setUsageFlags(imgUsageFlags);

    auto imgViewDesc = ll::ImageViewDescriptor{}
                           .setFilterMode(ll::ImageFilterMode::Nearest)
                           .setAddressMode(ll::ImageAddressMode::ClampToEdge)
                           .setNormalizedCoordinates(false)
                           .setIsSampled(false);

    auto imageView = ll::createAndInitImageView(session, memory, desc, imgViewDesc, ll::ImageLayout::General);
    REQUIRE(imageView != nullptr);

    auto portDescriptor = ll::PortDescriptor{0, "in_img_view", ll::PortDirection::In, ll::PortType::ImageView};
    portDescriptor.checkImageChannelTypeIs(ll::ChannelType::Float32);

    auto result = portDescriptor.isValid(imageView);
    REQUIRE(result.first == false);
    REQUIRE(result.second == "Port {binding: 0, name: in_img_view, portType: ll::PortType::ImageView} invalid image channel type, expecting any of: [Float32, ] got: Uint8");

    REQUIRE_FALSE(session->hasReceivedVulkanWarningMessages());
}

TEST_CASE("CheckImageViewChannelTypeAnyOf_positive", "test_PortDescriptor") {

    auto session = ll::Session::create(ll::SessionDescriptor().enableDebug(true));
    REQUIRE(session != nullptr);

    auto memory = session->createMemory(ll::MemoryPropertyFlagBits::DeviceLocal, 0, false);
    REQUIRE(memory != nullptr);

    const ll::ImageUsageFlags imgUsageFlags = {ll::ImageUsageFlagBits::Storage | ll::ImageUsageFlagBits::Sampled | ll::ImageUsageFlagBits::TransferDst};

    auto desc = ll::ImageDescriptor{}
                    .setWidth(640)
                    .setHeight(480)
                    .setChannelType(ll::ChannelType::Uint8)
                    .setChannelCount(ll::ChannelCount::C4)
                    .setUsageFlags(imgUsageFlags);

    auto imgViewDesc = ll::ImageViewDescriptor{}
                           .setFilterMode(ll::ImageFilterMode::Nearest)
                           .setAddressMode(ll::ImageAddressMode::ClampToEdge)
                           .setNormalizedCoordinates(false)
                           .setIsSampled(false);

    auto imageView = ll::createAndInitImageView(session, memory, desc, imgViewDesc, ll::ImageLayout::General);
    REQUIRE(imageView != nullptr);

    auto portDescriptor = ll::PortDescriptor{0, "in_img_view", ll::PortDirection::In, ll::PortType::ImageView};
    portDescriptor.checkImageChannelTypeIsAnyOf({ll::ChannelType::Uint16, ll::ChannelType::Uint8, ll::ChannelType::Uint32});

    auto result = portDescriptor.isValid(imageView);
    REQUIRE(result.first == true);
    REQUIRE(result.second == "");

    REQUIRE_FALSE(session->hasReceivedVulkanWarningMessages());
}

TEST_CASE("CheckImageViewChannelTypeAnyOf_negative", "test_PortDescriptor") {

    auto session = ll::Session::create(ll::SessionDescriptor().enableDebug(true));
    REQUIRE(session != nullptr);

    auto memory = session->createMemory(ll::MemoryPropertyFlagBits::DeviceLocal, 0, false);
    REQUIRE(memory != nullptr);

    const ll::ImageUsageFlags imgUsageFlags = {ll::ImageUsageFlagBits::Storage | ll::ImageUsageFlagBits::Sampled | ll::ImageUsageFlagBits::TransferDst};

    auto desc = ll::ImageDescriptor{}
                    .setWidth(640)
                    .setHeight(480)
                    .setChannelType(ll::ChannelType::Uint8)
                    .setChannelCount(ll::ChannelCount::C4)
                    .setUsageFlags(imgUsageFlags);

    auto imgViewDesc = ll::ImageViewDescriptor{}
                           .setFilterMode(ll::ImageFilterMode::Nearest)
                           .setAddressMode(ll::ImageAddressMode::ClampToEdge)
                           .setNormalizedCoordinates(false)
                           .setIsSampled(false);

    auto imageView = ll::createAndInitImageView(session, memory, desc, imgViewDesc, ll::ImageLayout::General);
    REQUIRE(imageView != nullptr);

    auto portDescriptor = ll::PortDescriptor{0, "in_img_view", ll::PortDirection::In, ll::PortType::ImageView};
    portDescriptor.checkImageChannelTypeIsAnyOf({ll::ChannelType::Float16, ll::ChannelType::Float32, ll::ChannelType::Float64});
    
    auto result = portDescriptor.isValid(imageView);
    REQUIRE(result.first == false);
    REQUIRE(result.second == "Port {binding: 0, name: in_img_view, portType: ll::PortType::ImageView} invalid image channel type, expecting any of: [Float16, Float32, Float64, ] got: Uint8");

    REQUIRE_FALSE(session->hasReceivedVulkanWarningMessages());
}

TEST_CASE("CheckImageViewNormalizedCoordinates_positive", "test_PortDescriptor") {

    auto session = ll::Session::create(ll::SessionDescriptor().enableDebug(true));
    REQUIRE(session != nullptr);

    auto memory = session->createMemory(ll::MemoryPropertyFlagBits::DeviceLocal, 0, false);
    REQUIRE(memory != nullptr);

    const ll::ImageUsageFlags imgUsageFlags = {ll::ImageUsageFlagBits::Storage | ll::ImageUsageFlagBits::Sampled | ll::ImageUsageFlagBits::TransferDst};

    auto desc = ll::ImageDescriptor{}
                    .setWidth(640)
                    .setHeight(480)
                    .setChannelType(ll::ChannelType::Uint8)
                    .setChannelCount(ll::ChannelCount::C4)
                    .setUsageFlags(imgUsageFlags);

    auto imgViewDesc = ll::ImageViewDescriptor{}
                           .setFilterMode(ll::ImageFilterMode::Nearest)
                           .setAddressMode(ll::ImageAddressMode::ClampToEdge)
                           .setNormalizedCoordinates(false)
                           .setIsSampled(false);

    auto imageView = ll::createAndInitImageView(session, memory, desc, imgViewDesc, ll::ImageLayout::General);
    REQUIRE(imageView != nullptr);

    auto portDescriptor = ll::PortDescriptor{0, "in_img_view", ll::PortDirection::In, ll::PortType::ImageView};
    portDescriptor.checkImageViewNormalizedCoordinatesIs(false);

    auto result = portDescriptor.isValid(imageView);
    REQUIRE(result.first == true);
    REQUIRE(result.second == "");

    REQUIRE_FALSE(session->hasReceivedVulkanWarningMessages());
}

TEST_CASE("CheckImageViewNormalizedCoordinates_negative", "test_PortDescriptor") {

    auto session = ll::Session::create(ll::SessionDescriptor().enableDebug(true));
    REQUIRE(session != nullptr);

    auto memory = session->createMemory(ll::MemoryPropertyFlagBits::DeviceLocal, 0, false);
    REQUIRE(memory != nullptr);

    const ll::ImageUsageFlags imgUsageFlags = {ll::ImageUsageFlagBits::Storage | ll::ImageUsageFlagBits::Sampled | ll::ImageUsageFlagBits::TransferDst};

    auto desc = ll::ImageDescriptor{}
                    .setWidth(640)
                    .setHeight(480)
                    .setChannelType(ll::ChannelType::Uint8)
                    .setChannelCount(ll::ChannelCount::C4)
                    .setUsageFlags(imgUsageFlags);

    auto imgViewDesc = ll::ImageViewDescriptor{}
                           .setFilterMode(ll::ImageFilterMode::Nearest)
                           .setAddressMode(ll::ImageAddressMode::ClampToEdge)
                           .setNormalizedCoordinates(false)
                           .setIsSampled(false);

    auto imageView = ll::createAndInitImageView(session, memory, desc, imgViewDesc, ll::ImageLayout::General);
    REQUIRE(imageView != nullptr);

    auto portDescriptor = ll::PortDescriptor{0, "in_img_view", ll::PortDirection::In, ll::PortType::ImageView};
    portDescriptor.checkImageViewNormalizedCoordinatesIs(true);

    auto result = portDescriptor.isValid(imageView);
    REQUIRE(result.first == false);
    REQUIRE(result.second == "Port {binding: 0, name: in_img_view, portType: ll::PortType::ImageView} invalid image view normalized coordinates flag, expecting: 1 got: 0");

    REQUIRE_FALSE(session->hasReceivedVulkanWarningMessages());
}

TEST_CASE("CheckSampledImageView", "test_PortDescriptor") {

    auto session = ll::Session::create(ll::SessionDescriptor().enableDebug(true));
    REQUIRE(session != nullptr);

    auto memory = session->createMemory(ll::MemoryPropertyFlagBits::DeviceLocal, 0, false);
    REQUIRE(memory != nullptr);

    const ll::ImageUsageFlags imgUsageFlags = {ll::ImageUsageFlagBits::Storage | ll::ImageUsageFlagBits::Sampled | ll::ImageUsageFlagBits::TransferDst};

    auto desc = ll::ImageDescriptor{}
                    .setWidth(640)
                    .setHeight(480)
                    .setChannelType(ll::ChannelType::Uint8)
                    .setChannelCount(ll::ChannelCount::C1)
                    .setUsageFlags(imgUsageFlags);

    auto imgViewDesc = ll::ImageViewDescriptor{}
                           .setFilterMode(ll::ImageFilterMode::Nearest)
                           .setAddressMode(ll::ImageAddressMode::ClampToEdge)
                           .setNormalizedCoordinates(false)
                           .setIsSampled(true);

    auto imageView = ll::createAndInitImageView(session, memory, desc, imgViewDesc, ll::ImageLayout::General);
    REQUIRE(imageView != nullptr);

    auto portDescriptor = ll::PortDescriptor{0, "in_img_view", ll::PortDirection::In, ll::PortType::SampledImageView};

    auto result = portDescriptor.isValid(imageView);
    REQUIRE(result.first == true);
    REQUIRE(result.second == "");

    REQUIRE_FALSE(session->hasReceivedVulkanWarningMessages());
}

TEST_CASE("CheckSampledImageViewIncorrectPortType", "test_PortDescriptor") {

    auto session = ll::Session::create(ll::SessionDescriptor().enableDebug(true));
    REQUIRE(session != nullptr);

    auto memory = session->createMemory(ll::MemoryPropertyFlagBits::DeviceLocal, 0, false);
    REQUIRE(memory != nullptr);

    const ll::ImageUsageFlags imgUsageFlags = {ll::ImageUsageFlagBits::Storage | ll::ImageUsageFlagBits::Sampled | ll::ImageUsageFlagBits::TransferDst};

    auto desc = ll::ImageDescriptor{}
                    .setWidth(640)
                    .setHeight(480)
                    .setChannelType(ll::ChannelType::Uint8)
                    .setChannelCount(ll::ChannelCount::C1)
                    .setUsageFlags(imgUsageFlags);

    auto imgViewDesc = ll::ImageViewDescriptor{}
                           .setFilterMode(ll::ImageFilterMode::Nearest)
                           .setAddressMode(ll::ImageAddressMode::ClampToEdge)
                           .setNormalizedCoordinates(false)
                           .setIsSampled(true);

    auto imageView = ll::createAndInitImageView(session, memory, desc, imgViewDesc, ll::ImageLayout::General);
    REQUIRE(imageView != nullptr);

    auto portDescriptor = ll::PortDescriptor{0, "in_img_view", ll::PortDirection::In, ll::PortType::ImageView};

    auto result = portDescriptor.isValid(imageView);
    REQUIRE(result.first == false);
    REQUIRE(result.second == "Port {binding: 0, name: in_img_view, portType: ll::PortType::ImageView} cannot receive object of type ll::PortType::SampledImageView");

    REQUIRE_FALSE(session->hasReceivedVulkanWarningMessages());
}

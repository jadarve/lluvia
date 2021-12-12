#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "lluvia/core.h"
#include <vulkan/vulkan.hpp>
#include <iostream>
#include <memory>

TEST_CASE("CreateMemory", "test_createMemory")
{

    auto session = ll::Session::create(ll::SessionDescriptor().enableDebug(true));
    REQUIRE(session != nullptr);

    const ll::MemoryPropertyFlags deviceFlags = ll::MemoryPropertyFlagBits::DeviceLocal;

    std::shared_ptr<ll::Memory> deviceMemory = session->createMemory(deviceFlags, 32*1024*1024, false);
    REQUIRE(deviceMemory != nullptr);


    std::shared_ptr<ll::Buffer> buffer = deviceMemory->createBuffer(1024);
    REQUIRE(buffer != nullptr);

    // 32x32 image with one uint8 color channel per pixel
    ll::ImageDescriptor desc = ll::ImageDescriptor(1, 32, 32, ll::ChannelCount::C1, ll::ChannelType::Uint8);
    std::shared_ptr<ll::Image> image = deviceMemory->createImage(desc);
    REQUIRE(image != nullptr);

    std::cout << "buffer:" << std::endl;
    std::cout << "  page         : " << buffer->getAllocationInfo().page << std::endl;
    std::cout << "  offset       : " << buffer->getAllocationInfo().offset << std::endl;
    std::cout << "  left padding : " << buffer->getAllocationInfo().leftPadding << std::endl;
    std::cout << "  size         : " << buffer->getAllocationInfo().size << std::endl;

    std::cout << std::endl;
    std::cout << "image:" << std::endl;
    std::cout << "  page         : " << image->getAllocationInfo().page << std::endl;
    std::cout << "  offset       : " << image->getAllocationInfo().offset << std::endl;
    std::cout << "  left padding : " << image->getAllocationInfo().leftPadding << std::endl;
    std::cout << "  size         : " << image->getAllocationInfo().size << std::endl;
}

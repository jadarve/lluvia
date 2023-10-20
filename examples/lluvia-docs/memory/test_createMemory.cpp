#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "lluvia/core.h"
#include <vulkan/vulkan.hpp>
#include <iostream>
#include <memory>

TEST_CASE("CreateMemory", "test_createMemory") {

    auto session = ll::Session::create(ll::SessionDescriptor().enableDebug(true));
    REQUIRE(session != nullptr);

    const ll::MemoryPropertyFlags deviceFlags = ll::MemoryPropertyFlagBits::DeviceLocal;
    const ll::MemoryPropertyFlags hostFlags = ll::MemoryPropertyFlagBits::DeviceLocal |
                                              ll::MemoryPropertyFlagBits::HostVisible |
                                              ll::MemoryPropertyFlagBits::HostCoherent;

    std::shared_ptr<ll::Memory> deviceMemory = session->createMemory(deviceFlags, 32 * 1024 * 1024, false);

    std::shared_ptr<ll::Memory> hostMemory = session->createMemory(hostFlags, 32 * 1024 * 1024, false);

    REQUIRE(deviceMemory != nullptr);
    REQUIRE(hostMemory != nullptr);

    // print memory properties
    std::cout << "DeviceMemory" << std::endl;
    // std::cout << "    flags      : " << vk::to_string(deviceMemory->getMemoryPropertyFlags()) << std::endl;
    std::cout << "    isMappable : " << deviceMemory->isMappable() << std::endl;
    std::cout << "    pageCount  : " << deviceMemory->getPageCount() << std::endl;
    std::cout << "    pageSize   : " << deviceMemory->getPageSize() << std::endl;

    std::cout << std::endl;
    std::cout << "HostMemory" << std::endl;
    // std::cout << "    flags      : " << vk::to_string(hostMemory->getMemoryPropertyFlags()) << std::endl;
    std::cout << "    isMappable : " << hostMemory->isMappable() << std::endl;
    std::cout << "    pageCount  : " << hostMemory->getPageCount() << std::endl;
    std::cout << "    pageSize   : " << hostMemory->getPageSize() << std::endl;
}

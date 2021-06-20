#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include <iostream>
#include <vector>
#include "lluvia/core.h"

#include <vulkan/vulkan.hpp>

TEST_CASE("DebugEnabled", "test_enumerateMemories")
{

    ll::SessionDescriptor desc = ll::SessionDescriptor().enableDebug(true);

    std::shared_ptr<ll::Session> session = ll::Session::create(desc);
    REQUIRE(session != nullptr);

    std::vector<vk::MemoryPropertyFlags> flagsVector = session->getSupportedMemoryFlags();

    for(int i = 0; i < flagsVector.size(); ++i) {
        const vk::MemoryPropertyFlags &flags = flagsVector[i];
        
        std::cout << "Memory index: " << i << std::endl;
        std::cout << "    Supported flags: " << vk::to_string(flags) << std::endl;
    }
}

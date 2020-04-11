/**
@file       test_BufferCopy.h
@brief      Test buffer mapping operations.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include <iostream>
#include <system_error>

#include "lluvia/core.h"

using memflags = vk::MemoryPropertyFlagBits;


TEST_CASE("DifferentPage", "test_BufferMapping") {

    auto session = ll::Session::create();

    const auto hostMemFlags   = memflags::eHostVisible | memflags::eHostCoherent;;

    constexpr const auto bufferSize = 256u;
        
    auto hostMemory = session->createMemory(hostMemFlags, bufferSize, false);
    REQUIRE(hostMemory != nullptr);
    REQUIRE(hostMemory->isMappable() == true);

    auto buffer1 = hostMemory->createBuffer(bufferSize);
    REQUIRE(buffer1 != nullptr);

    auto buffer2 = hostMemory->createBuffer(bufferSize);
    REQUIRE(buffer2 != nullptr);

    // verify that the buffers are allocated in different memory pages
    REQUIRE(buffer1->getAllocationInfo().page != buffer2->getAllocationInfo().page);

    // calling the two map() in sequence should work as the buffers are allocated
    // in different memory pages
    auto ptr1 = buffer1->map<uint8_t>();
    auto ptr2 = buffer2->map<uint8_t>();
}


TEST_CASE("SamePage", "test_BufferMapping") {

    auto session = ll::Session::create();
    REQUIRE(session != nullptr);

    const auto hostMemFlags   = memflags::eHostVisible | memflags::eHostCoherent;;

    constexpr const auto pageSize   = 1024u;
    constexpr const auto bufferSize = pageSize / 2;
        
    auto hostMemory = session->createMemory(hostMemFlags, pageSize, false);
    REQUIRE(hostMemory != nullptr);
    REQUIRE(hostMemory->isMappable() == true);

    auto buffer1 = hostMemory->createBuffer(bufferSize);
    REQUIRE(buffer1 != nullptr);

    auto buffer2 = hostMemory->createBuffer(bufferSize);
    REQUIRE(buffer2 != nullptr);

    // verify that the buffers are allocated in different memory pages
    REQUIRE(buffer1->getAllocationInfo().page == buffer2->getAllocationInfo().page);

    auto ptr1 = buffer1->map<uint8_t>();

    // since both buffers are allocated in the same memory page, mapping
    // the second one should throw an exception
    REQUIRE_THROWS_AS(buffer2->map<uint8_t>(), std::system_error);
}


TEST_CASE("MapAndSet", "test_BufferMapping") {

    auto session = ll::Session::create();
    REQUIRE(session != nullptr);

    std::cout << "Getting host memory" << std::endl;

    auto uniformMemory = session->getHostMemory();
    REQUIRE(uniformMemory != nullptr);

    using params = struct {
        int a;
        float b;
    };

    const auto p = params{789456, 3.1415};
    

    auto uniformBuffer = uniformMemory->createBuffer(sizeof(p), vk::BufferUsageFlagBits::eUniformBuffer);
    REQUIRE(uniformBuffer != nullptr);
    
    uniformBuffer->mapAndSet(p);

    auto pGet = params{};

    {
        auto mapPtr = uniformBuffer->map<params>();
        pGet = *mapPtr;
    }

    REQUIRE(p.a == pGet.a);
    REQUIRE(p.b == pGet.b);
}

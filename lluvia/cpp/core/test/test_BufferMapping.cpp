/**
@file       test_BufferCopy.h
@brief      Test buffer mapping operations.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include <cstdint>
#include <iostream>
#include <system_error>
#include <vector>

#include "lluvia/core.h"

using memflags = ll::MemoryPropertyFlagBits;

TEST_CASE("DifferentPage", "test_BufferMapping")
{

    auto session = ll::Session::create(ll::SessionDescriptor().enableDebug(true));

    const auto hostMemFlags = memflags::HostVisible | memflags::HostCoherent;
    ;

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

    REQUIRE_FALSE(session->hasReceivedVulkanWarningMessages());
}

TEST_CASE("SamePage", "test_BufferMapping")
{

    auto session = ll::Session::create(ll::SessionDescriptor().enableDebug(true));
    REQUIRE(session != nullptr);

    const auto hostMemFlags = memflags::HostVisible | memflags::HostCoherent;
    ;

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

    REQUIRE_FALSE(session->hasReceivedVulkanWarningMessages());
}

TEST_CASE("MapAndSet", "test_BufferMapping")
{

    auto session = ll::Session::create(ll::SessionDescriptor().enableDebug(true));
    REQUIRE(session != nullptr);

    auto uniformMemory = session->getHostMemory();
    REQUIRE(uniformMemory != nullptr);

    using params = struct {
        int   a;
        float b;
    };

    const auto p = params {789456, 3.1415};

    auto uniformBuffer = uniformMemory->createBuffer(sizeof(p), ll::BufferUsageFlagBits::UniformBuffer);
    REQUIRE(uniformBuffer != nullptr);

    uniformBuffer->mapAndSet(p);

    auto pGet = params {};

    {
        auto mapPtr = uniformBuffer->map<params>();
        pGet        = *mapPtr;
    }

    REQUIRE(p.a == pGet.a);
    REQUIRE(p.b == pGet.b);

    REQUIRE_FALSE(session->hasReceivedVulkanWarningMessages());
}

TEST_CASE("MapAndSetFromVector", "test_BufferMapping")
{
    ///////////////////////////////////////////////////////
    // Given a session and a host memory
    auto session = ll::Session::create(ll::SessionDescriptor().enableDebug(true));
    REQUIRE(session != nullptr);

    auto hostMemory = session->getHostMemory();
    REQUIRE(hostMemory != nullptr);

    // and a vector of data
    const auto data = std::vector<uint8_t> {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};

    ///////////////////////////////////////////////////////
    // when a buffer is created with the size of the vector
    auto buffer = hostMemory->createBuffer(data.size() * sizeof(uint8_t));
    REQUIRE(buffer != nullptr);

    // and the vector is mapped and set into the buffer
    REQUIRE_NOTHROW(buffer->mapAndSetFromVector(data));

    ///////////////////////////////////////////////////////
    // then the buffer should contain the same data as the vector
    {
        auto mapPtr = buffer->map<uint8_t[]>();
        REQUIRE(mapPtr != nullptr);

        for (auto i = 0u; i < data.size(); ++i) {
            REQUIRE(mapPtr[i] == data[i]);
        }
    }

    REQUIRE_FALSE(session->hasReceivedVulkanWarningMessages());
}

/**
 * \file test_BufferCopy.cpp
 * \brief test buffer copy
 * \copyright 2017, Juan David Adarve. See AUTHORS for more details
 * \license Apache 2.0, see LICENSE for more details
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include <iostream>
#include "lluvia/core.h"

using memflags = vk::MemoryPropertyFlagBits;


TEST_CASE("HostToDeviceToHost", "BufferCopyTest") {

    constexpr const auto length = 128u;
    constexpr const auto bufferSize = length*sizeof(int);

    auto session = ll::Session::create();
    const auto hostMemFlags   = memflags::eHostVisible | memflags::eHostCoherent;;
    const auto deviceMemFlags = memflags::eDeviceLocal;
        
    auto hostMemory = session->createMemory(hostMemFlags, bufferSize, false);
    REQUIRE(hostMemory != nullptr);
    REQUIRE(hostMemory->isMappable() == true);

    auto hostBuffer = hostMemory->createBuffer(bufferSize);
    REQUIRE(hostBuffer != nullptr);
    REQUIRE(hostBuffer->isMappable() == true);

    auto deviceMemory = session->createMemory(deviceMemFlags, bufferSize, false);
    REQUIRE(deviceMemory != nullptr);
    // deviceMemory can be mappable on platforms with shared memory between host and device.

    auto deviceBuffer = deviceMemory->createBuffer(bufferSize);
    REQUIRE(deviceBuffer != nullptr);

    // fill the host buffer with some values
    {
        auto hostPtr = hostBuffer->map<int[]>();
        REQUIRE(hostPtr != nullptr);
        for (auto i = 0u; i < length; ++i) {
            hostPtr[i] = i;
        }
    } // unmap hostPtr

    // issue the copy command
    auto cmdBuffer1 = session->createCommandBuffer();

    cmdBuffer1->begin();
    cmdBuffer1->copyBuffer(*hostBuffer, *deviceBuffer);
    cmdBuffer1->end();

    session->run(*cmdBuffer1);


    // create a second host memory to copy the deviceBuffer into and check
    // if the content is equal to hostBuffer
    auto secMemory = session->createMemory(hostMemFlags, bufferSize, false);
    REQUIRE(secMemory != nullptr);

    auto secBuffer = secMemory->createBuffer(bufferSize);
    REQUIRE(secBuffer != nullptr);

    // session->copyBuffer(*deviceBuffer, *secBuffer);
    auto cmdBuffer2 = session->createCommandBuffer();

    cmdBuffer2->begin();
    cmdBuffer2->copyBuffer(*deviceBuffer, *secBuffer);
    cmdBuffer2->end();
    
    session->run(*cmdBuffer2);


    // compare host and secondary values. If they are equal, then it
    // means that the memory content of deviceBuffer is also equal.
    {
        auto hostPtr = hostBuffer->map<int[]>();
        auto secPtr  = secBuffer->map<int[]>();

        REQUIRE(hostPtr != nullptr);
        REQUIRE(secPtr != nullptr);

        auto areEqual = true;
        for (auto i = 0u; i < length; ++i) {

            const auto& hostValue = hostPtr[i];
            const auto& secValue = secPtr[i];

            if (hostValue != secValue) {
                std::cout << "values not equal at: " << i << ": " << hostPtr[i] << " != " << secPtr[i] << std::endl;
                areEqual = false;
                break;    
            }
        }

        REQUIRE(areEqual == true);
    } // unmap hostPtr and secPtr
}


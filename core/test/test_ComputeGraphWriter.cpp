/**
 * \file test_ComputeNode.cpp
 * \brief test compute node.
 * \copyright 2017, Juan David Adarve. See AUTHORS for more details
 * \license Apache 2.0, see LICENSE for more details
 */

#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"

#include "coreTestConfig.h"

#include <iostream>
#include "lluvia/core.h"


using memflags = vk::MemoryPropertyFlagBits;


TEST_CASE("MemoryAndBuffers", "test_ComputeGraphWriter") {

    auto session = ll::Session::create();
    REQUIRE(session != nullptr);

    auto hostMemory = session->createMemory(memflags::eHostVisible | memflags::eHostCoherent, 1024, false);
    REQUIRE(hostMemory != nullptr);

    auto devMemory = session->createMemory(memflags::eDeviceLocal, 4096, false);
    REQUIRE(devMemory != nullptr);

    auto hb_0 = hostMemory->createBuffer(512);  REQUIRE(hb_0 != nullptr);
    auto hb_1 = hostMemory->createBuffer(1024); REQUIRE(hb_1 != nullptr);
    auto hb_2 = hostMemory->createBuffer(256);  REQUIRE(hb_2 != nullptr);


    auto db_0 = hostMemory->createBuffer(128);  REQUIRE(db_0 != nullptr);
    auto db_1 = hostMemory->createBuffer(32);   REQUIRE(db_1 != nullptr);
    auto db_2 = hostMemory->createBuffer(768);  REQUIRE(db_2 != nullptr);
    auto db_3 = hostMemory->createBuffer(2048); REQUIRE(db_3 != nullptr);
    auto db_4 = hostMemory->createBuffer(4096); REQUIRE(db_4 != nullptr);



    auto graph = std::make_shared<ll::ComputeGraph>();
    graph->addMemory("hostMemory", hostMemory);
    graph->addMemory("devMemory", devMemory);

    graph->addBuffer("hb_0", hb_0);
    graph->addBuffer("hb_1", hb_1);
    graph->addBuffer("hb_2", hb_2);

    graph->addBuffer("db_0", db_0);
    graph->addBuffer("db_1", db_1);
    graph->addBuffer("db_2", db_2);
    graph->addBuffer("db_3", db_3);
    graph->addBuffer("db_4", db_4);


    auto writer = ll::ComputeGraphFileWriter {};

    writer.write(graph, "moni moni");
}

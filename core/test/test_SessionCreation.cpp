/**
 * \file test_SessionCreation.cpp
 * \brief test Session creation
 * \copyright 2017, Juan David Adarve. See AUTHORS for more details
 * \license Apache 2.0, see LICENSE for more details
 */

#include <iostream>
#include <gtest/gtest.h>
#include "lluvia/core.h"

TEST(SessionCreationTest, DefaultParameters) {

    ll::Session session {};

    // check that the instance is created successfully
    // ASSERT_EQ(result, vk::Result::eSuccess);
}

int main(int argc, char **argv) {
    
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
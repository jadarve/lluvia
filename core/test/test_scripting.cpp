/**
 * \file test_scripting.cpp
 * \brief test scripting interface
 * \copyright 2018, Juan David Adarve. See AUTHORS for more details
 * \license Apache 2.0, see LICENSE for more details
 */

#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"

#include "lluvia/core.h"

#include <chaiscript/chaiscript.hpp>

#include <cstdint>
#include <iostream>
#include <memory>
#include <vector>
#include <string>

struct bufferDesc {
    uint64_t size;
};

struct imgDesc {
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    ll::ChannelCount channelCount;
    std::string channelType;
    std::vector<std::string> usageFlags;
    // missing: tiling
};


TEST_CASE("CreateInstance", "test_scripting") {

    chaiscript::ChaiScript chai {};

    // base conversions
    chai.add(chaiscript::vector_conversion<std::vector<std::string>>());

    // base types
    chai.add(chaiscript::bootstrap::standard_library::vector_type<std::vector<std::string>>("vector_string"));
    // chai.add(chaiscript::bootstrap::standard_library::vector_type<std::vector<uint32_t>>("vector_uint32_t"));

    // buffer
    chai.add(chaiscript::fun(&bufferDesc::size), "size");
    chai.add(chaiscript::constructor<bufferDesc()>(), "bufferDesc");
    chai.add(chaiscript::user_type<bufferDesc>(), "bufferDesc");

    // imgDesc
    chai.add(chaiscript::fun(&imgDesc::width), "width");
    chai.add(chaiscript::fun(&imgDesc::height), "height");
    chai.add(chaiscript::fun(&imgDesc::depth), "depth");
    chai.add(chaiscript::fun(&imgDesc::channelCount), "channelCount");
    chai.add(chaiscript::fun(&imgDesc::channelType), "channelType");
    chai.add(chaiscript::fun(&imgDesc::usageFlags), "usageFlags");
    chai.add(chaiscript::constructor<imgDesc()>(), "imgDesc");
    chai.add(chaiscript::user_type<imgDesc>(), "imgDesc");

    chai.eval(R""(
        auto b = bufferDesc()
        b.size = 1024
        print(b.size)

        auto img = imgDesc()
        img.width = 640
        img.height = 480
        img.depth = 1
        img.channelType = "uint8"
        img.usageFlags = ["Storage", "Sampled"]

        print([img.width, img.height, img.depth])
        )"");
}


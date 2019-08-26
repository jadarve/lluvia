
#include <lluvia/core.h>

#include "ImagePyramid.h"

#include "CLI11/CLI11.hpp"

#include <iostream>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>

#include <vulkan/vulkan.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>


using image_t = struct {
    int32_t width;
    int32_t height;
    int32_t channels;
    std::vector<uint8_t> data;
};


image_t readImage(const std::string& filepath) {

    // read image data
    auto img        = image_t {};
    stbi_uc* pixels = stbi_load(filepath.c_str(), &img.width, &img.height, &img.channels, STBI_rgb_alpha);
    img.channels    = 4; // overwrite to 4

    const auto imageSize = static_cast<uint64_t>(img.width * img.height * img.channels);

    if (!pixels) {
        throw std::runtime_error("failed to read image file: " + filepath);
    }

    img.data.resize(imageSize);
    std::memcpy(img.data.data(), pixels, imageSize);
    stbi_image_free(pixels);

    return img;
}


int main(int argc, char const* argv[])
{
    CLI::App app{"Image pyramid with lua nodes"};

    auto filename = std::string {};

    app.add_option("filename", filename, "image file name")->required();

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        return app.exit(e);
    }

    std::cout << "filename: " << filename << std::endl;

    const auto image     = readImage(filename);
    const auto imageSize = image.width*image.height*image.channels*ll::getChannelTypeSize(ll::ChannelType::Uint8);


    auto session = ll::Session::create();

    // register programs and node builders
    session->setProgram("imageDownsampleX", session->createProgram("imageDownsampleX.spv"));
    session->setProgram("imageDownsampleY", session->createProgram("imageDownsampleY.spv"));

    session->scriptFile("/home/jadarve/git/lluvia/samples/imagePyramid_lua/lua/imageDownsampleX.lua");
    session->scriptFile("/home/jadarve/git/lluvia/samples/imagePyramid_lua/lua/imageDownsampleY.lua");

    auto memory = session->createMemory(vk::MemoryPropertyFlagBits::eDeviceLocal, 0);

    const vk::ImageUsageFlags imgUsageFlags = { vk::ImageUsageFlagBits::eStorage
                                              | vk::ImageUsageFlagBits::eSampled
                                              | vk::ImageUsageFlagBits::eTransferDst
                                              | vk::ImageUsageFlagBits::eTransferSrc};

    const auto imgDesc = ll::ImageDescriptor {static_cast<uint32_t>(image.width),
                                              static_cast<uint32_t>(image.height),
                                              1,
                                              ll::ChannelCount::C4, ll::ChannelType::Uint8, imgUsageFlags};

    const auto viewDesc = ll::ImageViewDescriptor {}
                            .setIsSampled(false)
                            .setNormalizedCoordinates(false);

    auto in_RGBA = ll::createAndInitImageView(session, memory, imgDesc, viewDesc, vk::ImageLayout::eGeneral);


    // transfer image to in_RGBA
    // stage buffer
    const auto hostMemFlags = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
    auto hostMemory         = session->createMemory(hostMemFlags, imageSize);
    auto stageBuffer        = hostMemory->createBuffer(imageSize);

    // copy to stage buffer
    {
        auto mapPtr = stageBuffer->map<uint8_t>();
        std::memcpy(mapPtr.get(), image.data.data(), imageSize);
    } // unmap mapPtr

    // change input image layout to dst optimal
    // copy stage buffer to input image
    // change input image layout to general
    auto imgCopyCmdBuffer = session->createCommandBuffer();
    imgCopyCmdBuffer->begin();
    imgCopyCmdBuffer->changeImageLayout(*in_RGBA, vk::ImageLayout::eTransferDstOptimal);
    imgCopyCmdBuffer->copyBufferToImage(*stageBuffer, *in_RGBA->getImage());
    imgCopyCmdBuffer->changeImageLayout(*in_RGBA, vk::ImageLayout::eGeneral);
    imgCopyCmdBuffer->end();
    session->run(*imgCopyCmdBuffer);



    auto pyramid = ImagePyramid {session};
    pyramid.bind("in_RGBA", in_RGBA);
    pyramid.init();


    auto cmdBuffer = session->createCommandBuffer();
    cmdBuffer->begin();
    pyramid.record(*cmdBuffer);
    cmdBuffer->end();

    for (auto n = 0u; n < 1000; ++n) {
        const auto start = std::chrono::high_resolution_clock::now();
        session->run(*cmdBuffer);
        const auto end = std::chrono::high_resolution_clock::now();

        const auto diff = std::chrono::duration<float, std::micro> {end - start};
        std::cout << diff.count() << std::endl;
    }

    pyramid.writeAllImages(session);


    std::cout << "CPP: finish" << std::endl;

    return EXIT_SUCCESS;
}


#include <lluvia/core.h>

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

void writeImage(std::shared_ptr<ll::Session> session, std::shared_ptr<ll::Image> image, const std::string& filename) {

    const auto imageSize     = image->getSize();
    const auto currentLayout = image->getLayout();

    // create host visible memory
    const auto hostMemFlags = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
    auto hostMemory         = session->createMemory(hostMemFlags, imageSize);
    auto hostImage          = hostMemory->createBuffer(imageSize);

    auto cmdBuffer = session->createCommandBuffer();

    cmdBuffer->begin();
    cmdBuffer->changeImageLayout(*image, vk::ImageLayout::eTransferSrcOptimal);
    cmdBuffer->copyImageToBuffer(*image, *hostImage);
    cmdBuffer->changeImageLayout(*image, currentLayout);
    cmdBuffer->end();

    session->run(*cmdBuffer);

    auto mapPtr = hostImage->map<uint8_t>();
    const auto res = stbi_write_bmp(filename.c_str(),
        image->getWidth(),
        image->getHeight(),
        image->getChannelCount<uint32_t>(),
        mapPtr.get());

    std::cout << "stbi_write_bmp result: " << res << std::endl;
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

    auto session = ll::Session::create();

    // register programs and node builders
    session->setProgram("ImageDownsampleX", session->createProgram("ImageDownsampleX.spv"));
    session->setProgram("ImageDownsampleY", session->createProgram("ImageDownsampleY.spv"));

    session->scriptFile("/home/juan/workspace/git/lluvia/samples/imagePyramid_lua/lua/ImageDownsampleX.lua");
    session->scriptFile("/home/juan/workspace/git/lluvia/samples/imagePyramid_lua/lua/ImageDownsampleY.lua");
    session->scriptFile("/home/juan/workspace/git/lluvia/samples/imagePyramid_lua/lua/ImagePyramid.lua");
    
    auto pyramidDesc = session->createContainerNodeDescriptor("ImagePyramid");
    auto pyramid = session->createContainerNode(pyramidDesc);

    const auto image     = readImage(filename);
    const auto imageSize = image.width*image.height*image.channels*ll::getChannelTypeSize(ll::ChannelType::Uint8);

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


    pyramid->bind("in_RGBA", in_RGBA);
    pyramid->init();

    auto cmdBuffer = session->createCommandBuffer();
    cmdBuffer->begin();
    pyramid->record(*cmdBuffer);
    cmdBuffer->end();

    session->run(*cmdBuffer);

    constexpr const auto names = std::array<const char*, 4> {{"out_RGBA_downY_0", "out_RGBA_downY_1", "out_RGBA_downY_2", "out_RGBA_downY_3"}};

    for (const auto& name : names) {
        writeImage(session,
                   std::static_pointer_cast<ll::ImageView>(pyramid->getPort(name))->getImage(),
                   std::string{name} + std::string{".bmp"});
    }

    std::cout << "ImagePyramid_lua: finish" << std::endl;
    return EXIT_SUCCESS;
}


#include <lluvia/core.h>

#include <iostream>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>

#include <vulkan/vulkan.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#ifdef __linux__

// at least in Ubuntu 18.04 and clang 6 <filesystem> is still in experimental
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

#elif _WIN32

#include <filesystem>
namespace fs = std::filesystem;

#endif // OS switch


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

int main(int argc, char const* argv[]) {

    if (argc != 2) {
        std::cout << "missing image path, use 'imagePyramid <path to image>'" << std::endl;
        exit(-1);
    }

    const auto imagePath = fs::path {argv[1]};

    std::cout << "filename: " << imagePath << std::endl;

    auto session = ll::Session::create();

    // register programs and node builders
    session->setProgram("ImageDownsampleX", session->createProgram("samples/imagePyramid_lua/glsl/ImageDownsampleX.spv"));
    session->setProgram("ImageDownsampleY", session->createProgram("samples/imagePyramid_lua/glsl/ImageDownsampleY.spv"));

    session->scriptFile("samples/imagePyramid_lua/lua/ImageDownsampleX.lua");
    session->scriptFile("samples/imagePyramid_lua/lua/ImageDownsampleY.lua");
    session->scriptFile("samples/imagePyramid_lua/lua/ImagePyramid.lua");

    auto pyramidDesc = session->createContainerNodeDescriptor("ImagePyramid");
    auto pyramid = session->createContainerNode(pyramidDesc);

    const auto image     = readImage(imagePath.string());
    const auto imageSize = image.width*image.height*image.channels*ll::getChannelTypeSize(ll::ChannelType::Uint8);

    auto memory = session->createMemory(vk::MemoryPropertyFlagBits::eDeviceLocal, 0);

    const vk::ImageUsageFlags imgUsageFlags = { vk::ImageUsageFlagBits::eStorage
                                              | vk::ImageUsageFlagBits::eSampled
                                              | vk::ImageUsageFlagBits::eTransferDst
                                              | vk::ImageUsageFlagBits::eTransferSrc};

    auto imgDesc = ll::ImageDescriptor{1,
                                             static_cast<uint32_t>(image.height),
                                             static_cast<uint32_t>(image.width),
                                             ll::ChannelCount::C4, ll::ChannelType::Uint8};

    imgDesc.setUsageFlags(imgUsageFlags);

    const auto viewDesc = ll::ImageViewDescriptor{}
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

    const auto levels = pyramidDesc.getParameter("levels").get<int32_t>();
    const auto baseName = "out_RGBA_downY_";
    for (auto i = 0; i < levels; ++i) {

        const auto name = baseName + std::to_string(i);

        const auto outPath = fs::path{imagePath}.replace_filename(
            std::string{name} + std::string{".bmp"});

        writeImage(session,
                   std::static_pointer_cast<ll::ImageView>(pyramid->getPort(name))->getImage(),
                   outPath.string());
    }

    std::cout << "ImagePyramid_lua: finish" << std::endl;
    return EXIT_SUCCESS;
}

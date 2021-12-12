
#include "ImagePyramid.h"

#include <lluvia/core.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>
#include <memory>
#include <vector>
#include <iomanip>

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


image_t readImage(const fs::path& filepath) {

    // read image data
    auto img        = image_t {};
    stbi_uc* pixels = stbi_load((char*)filepath.c_str(), &img.width, &img.height, &img.channels, STBI_rgb_alpha);
    img.channels    = 4; // overwrite to 4

    const auto imageSize = static_cast<uint64_t>(img.width * img.height * img.channels);

    if (!pixels) {
        throw std::runtime_error("failed to read image file: " + filepath.string());
    }

    img.data.resize(imageSize);
    std::memcpy(img.data.data(), pixels, imageSize);
    stbi_image_free(pixels);

    return img;
}


int main(int argc, const char** argv) {

    if (argc != 2) {
        std::cout << "missing image path, use 'imagePyramid <path to image>'" << std::endl;
        exit(-1);
    }

    const auto imagePath = fs::path {argv[1]};

    const auto image = readImage(imagePath);

    const auto channelType        = ll::ChannelType::Uint8;
    const auto imageSize          = image.width*image.height*image.channels*ll::getChannelTypeSize(channelType);
    const auto inputImageMemFlags = ll::MemoryPropertyFlagBits::DeviceLocal;


    auto session = std::shared_ptr<ll::Session> {ll::Session::create()};

    auto memory = session->createMemory(inputImageMemFlags, imageSize, false);

    const auto imgFlags = vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eTransferSrc;

    auto imgDesc = ll::ImageDescriptor{1,
                                       static_cast<uint32_t>(image.height),
                                       static_cast<uint32_t>(image.width),
                                       ll::castChannelCount(image.channels),
                                       channelType};

    imgDesc.setUsageFlags(imgFlags);

        auto inputImage = memory->createImage(imgDesc);

    // stage buffer
    const auto hostMemFlags = ll::MemoryPropertyFlagBits::HostVisible | ll::MemoryPropertyFlagBits::HostCoherent;
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
    imgCopyCmdBuffer->changeImageLayout(*inputImage, vk::ImageLayout::eTransferDstOptimal);
    imgCopyCmdBuffer->copyBufferToImage(*stageBuffer, *inputImage);
    imgCopyCmdBuffer->changeImageLayout(*inputImage, vk::ImageLayout::eGeneral);
    imgCopyCmdBuffer->end();
    session->run(*imgCopyCmdBuffer);

    std::cout << "input image: [" << inputImage->getWidth() << ", " << inputImage->getHeight() << "]" << std::endl;

    auto imagePyramid = ImagePyramid {4};
    imagePyramid.setInputImage(inputImage);
    imagePyramid.init(session);


    auto cmdBuffer = session->createCommandBuffer();
    auto duration = session->createDuration();

    cmdBuffer->begin();
    cmdBuffer->durationStart(*duration);
    imagePyramid.record(*cmdBuffer);
    cmdBuffer->durationEnd(*duration);
    cmdBuffer->end();

    for (auto n = 0u; n < 1000; ++n) {

        session->run(*cmdBuffer);

        std::cout << std::setprecision(3) << duration->getNanoseconds() * 1e-6 << " ms\n";
    }

    imagePyramid.writeAllImages(session, imagePath);
}

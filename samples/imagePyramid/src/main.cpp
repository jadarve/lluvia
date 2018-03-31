
#include "ImagePyramid.h"

#include <lluvia/core.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <iostream>
#include <memory>
#include <vector>


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


int main() {

    const auto image = readImage("raton.jpg");

    const auto channelType        = ll::ChannelType::Uint8;
    const auto imageSize          = image.width*image.height*image.channels*ll::getChannelTypeSize(channelType);
    const auto inputImageMemFlags = vk::MemoryPropertyFlagBits::eDeviceLocal;
    

    auto session = std::shared_ptr<ll::Session> {ll::Session::create()};

    auto memory = session->createMemory(inputImageMemFlags, imageSize, false);

    auto imgDesc = ll::ImageDescriptor {static_cast<uint32_t>(image.width),
                                        static_cast<uint32_t>(image.height),
                                        1,
                                        static_cast<uint32_t>(image.channels),
                                        channelType};
    
    const auto imgFlags = vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eTransferSrc;

    auto inputImage = memory->createImage(imgDesc, imgFlags);

    // stage buffer
    const auto hostMemFlags = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
    auto hostMemory         = session->createMemory(hostMemFlags, imageSize);
    auto stageBuffer        = hostMemory->createBuffer(imageSize);

    // copy to stage buffer
    auto mapPtr = stageBuffer->map();
    std::memcpy(mapPtr, image.data.data(), imageSize);
    stageBuffer->unmap();


    // change input image layout to dst optimal
    // copy stage buffer to input image
    // change input image layout to general
    auto imgCopyCmdBuffer = session->createCommandBuffer();
    imgCopyCmdBuffer->begin();
    imgCopyCmdBuffer->changeImageLayout(*inputImage, vk::ImageLayout::eTransferDstOptimal);
    imgCopyCmdBuffer->copyBufferToImage(*stageBuffer, *inputImage);
    imgCopyCmdBuffer->changeImageLayout(*inputImage, vk::ImageLayout::eGeneral);
    imgCopyCmdBuffer->end();
    session->run(imgCopyCmdBuffer);
    

    auto imagePyramid = ImagePyramid {4};
    imagePyramid.setInputImage(inputImage);
    imagePyramid.init(session);


    auto cmdBuffer = session->createCommandBuffer();
    cmdBuffer->begin();
    imagePyramid.record(cmdBuffer);
    cmdBuffer->end();

    session->run(cmdBuffer);

    imagePyramid.writeAllImages(session);
}

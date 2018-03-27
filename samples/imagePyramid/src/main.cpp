
#include "ImagePyramid.h"

#include <lluvia/core.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <iostream>
#include <memory>
#include <vector>


void readImage(std::shared_ptr<ll::Session> session, std::shared_ptr<ll::Image> inputImage) {

    // read image data
    int32_t texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load("dog.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    uint64_t imageSize = texWidth * texHeight * 4;

    if (!pixels) {
        throw std::runtime_error("failed to load texture image!");
    }


    // stage buffer
    const auto hostMemFlags = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
    auto hostMemory         = session->createMemory(hostMemFlags, imageSize);
    auto stageBuffer        = hostMemory->createBuffer(imageSize);

    // copy to stage buffer
    auto mapPtr = stageBuffer->map();
    std::memcpy(mapPtr, pixels, imageSize);
    stageBuffer->unmap();

    stbi_image_free(pixels);


    // change input image layout to dst optimal
    // copy stage buffer to input image
    // change input image layout to general
    auto cmdBuffer = session->createCommandBuffer();
    cmdBuffer->begin();
    cmdBuffer->changeImageLayout(*inputImage, vk::ImageLayout::eTransferDstOptimal);
    cmdBuffer->copyBufferToImage(*stageBuffer, *inputImage);
    cmdBuffer->changeImageLayout(*inputImage, vk::ImageLayout::eGeneral);
    cmdBuffer->end();
    session->run(cmdBuffer);
}


int main() {

    const auto width       = 1280u;
    const auto height      = 850u;
    const auto channels    = 4;
    const auto channelType = ll::ChannelType::Uint8;

    const auto inputImageMemFlags = vk::MemoryPropertyFlagBits::eDeviceLocal;

    const auto memPageSize = width*height*channels*ll::getChannelTypeSize(channelType);

    auto session = std::shared_ptr<ll::Session> {ll::Session::create()};

    auto memory = session->createMemory(inputImageMemFlags, memPageSize, false);

    auto imgDesc = ll::ImageDescriptor {width, height, 1, channels, channelType};
    const auto imgFlags = vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eTransferSrc;
    auto inputImage = memory->createImage(imgDesc, imgFlags);

    // read input image and copy it to inputImage
    readImage(session, inputImage);
    

    auto imagePyramid = ImagePyramid {4};
    imagePyramid.setInputImage(inputImage);
    imagePyramid.init(session);


    auto cmdBuffer = session->createCommandBuffer();
    cmdBuffer->begin();
    imagePyramid.record(cmdBuffer);
    cmdBuffer->end();

    session->run(cmdBuffer);

    imagePyramid.writeAllImages(session);

    // writeImage(session, inputImage, 0);
}

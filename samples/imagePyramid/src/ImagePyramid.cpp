#include "ImagePyramid.h"

#include <iostream>

ImagePyramid::ImagePyramid(const uint32_t levels):
    levels {levels},
    inputImage {nullptr},
    memory {nullptr} {

    assert(levels >= 0);
}


void ImagePyramid::setInputImage(std::shared_ptr<ll::Image> inputImage) {
    assert(inputImage != nullptr);

    this->inputImage = inputImage;
}


void ImagePyramid::init(std::shared_ptr<ll::Session> session) {

    assert(inputImage != nullptr);
    assert(session != nullptr);

    std::cout << "ImagePyramid::init: levels: " << levels << std::endl;

    auto width       = inputImage->getWidth();
    auto height      = inputImage->getHeight();
    auto channels    = inputImage->getChannelCount();
    auto channelType = inputImage->getChannelType();

    const auto pageSize = uint64_t {inputImage->getSize()};
    const auto memFlags = vk::MemoryPropertyFlagBits::eDeviceLocal;

    memory = session->createMemory(memFlags, pageSize, false);
    assert(memory != nullptr);


    auto cmdBuffer = session->createCommandBuffer();
    cmdBuffer->begin();


    // create downsampled images
    auto imgDesc = ll::ImageDescriptor {width, height, 1, channels, channelType};
    auto imgViewDesc = ll::ImageViewDescriptor {}
                        .setIsSampled(false)
                        .setNormalizedCoordinates(false);

    // push the image view corresponding to the image pyramid at level 0
    // that is, the original resolution image
    imageViewsY.push_back(inputImage->createImageView(imgViewDesc));
    cmdBuffer->changeImageLayout(*inputImage, vk::ImageLayout::eGeneral);

    for (auto i = 0u; i < levels; ++i) {

        std::cout << "    width: " << width << " height: " << height << std::endl;

        width /= 2;
        imgDesc.setWidth(width);

        auto imgDownX = memory->createImage(imgDesc);
        auto imgViewDownX = imgDownX->createImageView(imgViewDesc);
        imageViewsX.push_back(imgViewDownX);
        cmdBuffer->changeImageLayout(*imgDownX, vk::ImageLayout::eGeneral);

        std::cout << "X: " << imgDownX->getAllocationInfo() << std::endl;


        height /= 2;
        imgDesc.setHeight(height);

        auto imgDownY = memory->createImage(imgDesc);
        auto imgViewDownY = imgDownY->createImageView(imgViewDesc);
        imageViewsY.push_back(imgViewDownY);
        cmdBuffer->changeImageLayout(*imgDownY, vk::ImageLayout::eGeneral);

        std::cout << "Y: " << imgDownY->getAllocationInfo() << std::endl;
    }


    // transition all the images to general image layout
    cmdBuffer->end();
    session->run(cmdBuffer);
}
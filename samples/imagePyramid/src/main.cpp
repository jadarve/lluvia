
#include "ImagePyramid.h"

#include <lluvia/core.h>

#include <iostream>
#include <memory>
#include <vector>


int main() {

    const auto width       = 640u;
    const auto height      = 480u;
    const auto channels    = 4;
    const auto channelType = ll::ChannelType::Uint8;

    const auto inputImageMemFlags = vk::MemoryPropertyFlagBits::eDeviceLocal;

    const auto memPageSize = width*height*channels*ll::getChannelTypeSize(channelType);

    auto session = std::shared_ptr<ll::Session> {ll::Session::create()};

    auto memory = session->createMemory(inputImageMemFlags, memPageSize, false);

    auto imgDesc = ll::ImageDescriptor {width, height, 1, channels, channelType};
    auto inputImage = memory->createImage(imgDesc);
    

    auto imagePyramid = ImagePyramid {3};
    imagePyramid.setInputImage(inputImage);
    imagePyramid.init(session);

}

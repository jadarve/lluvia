
#include "ImagePyramid.h"

#include <stb_image_write.h>

#include <array>
#include <iostream>


ImagePyramid::ImagePyramid(const std::shared_ptr<ll::Session>& session) :
    ll::ContainerNode(session) {
}


void ImagePyramid::record(ll::CommandBuffer& commandBuffer) const {

    for (auto h = 0u; h < 4; ++h) {
        auto downX = getNode("downX_" + std::to_string(h));
        auto downY = getNode("downY_" + std::to_string(h));

        downX->record(commandBuffer);
        commandBuffer.memoryBarrier();

        downY->record(commandBuffer);
        commandBuffer.memoryBarrier();
    }
}


void ImagePyramid::onInit() {

    auto in_RGBA = getPort("in_RGBA");

    // insert in_RGBA again as pyramid_0
    bind("pyramid_0", in_RGBA);

    std::cout << "changing layout" << std::endl;
    std::static_pointer_cast<ll::ImageView>(in_RGBA)->changeImageLayout(vk::ImageLayout::eGeneral);

    for (auto h = 0u; h < 4; ++h) {

        std::cout << "CPP: level: " << h  << std::endl;

        auto downX = m_session->createComputeNode("imageDownsampleX");
        auto downY = m_session->createComputeNode("imageDownsampleY");
        
        downX->bind("in_RGBA", in_RGBA);
        downX->init();
        
        downY->bind("in_RGBA", downX->getPort("out_RGBA"));
        downY->init();

        // the input to the next level will be the output of downY
        in_RGBA = downY->getPort("out_RGBA");
        bind("pyramid_" + std::to_string(h + 1), in_RGBA);

        bindNode("downX_" + std::to_string(h), downX);
        bindNode("downY_" + std::to_string(h), downY);
    }
}


void ImagePyramid::writeAllImages(std::shared_ptr<ll::Session> session) {

    constexpr const auto names = std::array<const char*, 4> {{"pyramid_0", "pyramid_1", "pyramid_2", "pyramid_3"}};

    for (const auto& name : names) {
        writeImage(session,
                   std::static_pointer_cast<ll::ImageView>(getPort(name))->getImage(),
                   std::string{name} + std::string{".bmp"});
    }
}


void ImagePyramid::writeImage(std::shared_ptr<ll::Session> session, std::shared_ptr<ll::Image> image, const std::string& filename) {

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

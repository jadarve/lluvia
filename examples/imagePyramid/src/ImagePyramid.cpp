#include "ImagePyramid.h"

#include "stb_image_write.h"

#include <iostream>

ImagePyramid::ImagePyramid(const uint32_t tLevels):
    levels {tLevels},
    inputImage {nullptr},
    memory {nullptr} {

    assert(levels >= 0);
}


void ImagePyramid::setInputImage(std::shared_ptr<ll::Image> tInputImage) {
    inputImage = tInputImage;
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
    const auto memFlags = ll::MemoryPropertyFlagBits::DeviceLocal;

    memory = session->createMemory(memFlags, pageSize, false);
    assert(memory != nullptr);


    auto cmdBuffer = session->createCommandBuffer();
    cmdBuffer->begin();


    // create downsampled images
    const auto imgFlags = ll::ImageUsageFlagBits::Storage | ll::ImageUsageFlagBits::TransferSrc;
    
    auto imgDesc = ll::ImageDescriptor {1, height, width, channels, channelType};
    imgDesc.setUsageFlags(imgFlags);

    auto imgViewDesc = ll::ImageViewDescriptor {}
                        .setIsSampled(false)
                        .setNormalizedCoordinates(false);

    // push the image view corresponding to the image pyramid at level 0
    // that is, the original resolution image
    imageViewsY.push_back(inputImage->createImageView(imgViewDesc));
    cmdBuffer->changeImageLayout(*inputImage, ll::ImageLayout::General);

    for (auto i = 0u; i < levels; ++i) {

        std::cout << "    width: " << width << " height: " << height << " channels: " << 
            static_cast<std::underlying_type_t<ll::ChannelCount>>(channels) << std::endl;

        width /= 2;
        imgDesc.setWidth(width);

        auto imgViewDownX = memory->createImageView(imgDesc, imgViewDesc);
        imageViewsX.push_back(imgViewDownX);
        cmdBuffer->changeImageLayout(*imgViewDownX, ll::ImageLayout::General);

        std::cout << "X: [" << imgViewDownX->getWidth() << ", " << imgViewDownX->getHeight() << ", " << imgViewDownX->getChannelCount<uint32_t>() << "]: " << imgViewDownX->getAllocationInfo() << std::endl;


        height /= 2;
        imgDesc.setHeight(height);

        auto imgViewDownY = memory->createImageView(imgDesc, imgViewDesc);
        imageViewsY.push_back(imgViewDownY);
        cmdBuffer->changeImageLayout(*imgViewDownY, ll::ImageLayout::General);

        std::cout << "Y: [" << imgViewDownY->getWidth() << ", " << imgViewDownY->getHeight() << "]: " << imgViewDownY->getAllocationInfo() << std::endl;
    }


    // transition all the images to general image layout
    cmdBuffer->end();
    session->run(*cmdBuffer);


    initComputeNodes(session);
}


void ImagePyramid::initComputeNodes(std::shared_ptr<ll::Session> session) {

    // Shader parameters
    //
    // layout(binding = 0, rgba8ui) uniform uimage2D inputImage;
    // layout(binding = 1, rgba8ui) uniform uimage2D outputImage;
    auto programX = session->createProgram("samples/imagePyramid/glsl/imageDownsampleX.comp.spv");
    auto programY = session->createProgram("samples/imagePyramid/glsl/imageDownsampleY.comp.spv");

    assert(programX != nullptr);
    assert(programY != nullptr);


    auto descX = ll::ComputeNodeDescriptor {}
        .setProgram(programX, "main")
        .setLocalShape({32, 32, 1})
        .addPorts({
            {0, "in_RGBA", ll::PortDirection::In, ll::PortType::ImageView},
            {1, "out_RGBA", ll::PortDirection::Out, ll::PortType::ImageView},
        });

    auto descY = ll::ComputeNodeDescriptor {}
        .setProgram(programY, "main")
        .setLocalShape({32, 32, 1})
        .addPorts({
            {0, "in_RGBA", ll::PortDirection::In, ll::PortType::ImageView},
            {1, "out_RGBA", ll::PortDirection::Out, ll::PortType::ImageView},
        });


    auto width  = inputImage->getWidth();
    auto height = inputImage->getHeight();

    for (auto i = 0u; i < levels; ++i) {

        width /= 2;

        auto descX_i = ll::ComputeNodeDescriptor {descX}
            .configureGridShape({width, height, 1});

        std::cout << i << ": grid  X: [" << descX_i.getGridX() << ", " << descX_i.getGridY() << ", " << descX_i.getGridZ() << "]" << std::endl;
        std::cout << i << ": local X: [" << descX_i.getLocalX() << ", " << descX_i.getLocalY() << ", " << descX_i.getLocalZ() << "]" << std::endl;

        auto nodeX = session->createComputeNode(descX_i);
        nodeX->bind("in_RGBA", imageViewsY[i]);
        nodeX->bind("out_RGBA", imageViewsX[i]);
        nodeX->init();
        computeNodesX.push_back(nodeX);

        height /= 2;

        auto descY_i = ll::ComputeNodeDescriptor {descY}
            .configureGridShape({width, height, 1});

        std::cout << i << ": grid  Y: [" << descY_i.getGridX() << ", " << descY_i.getGridY() << ", " << descY_i.getGridZ() << "]" << std::endl;
        std::cout << i << ": local Y: [" << descY_i.getLocalX() << ", " << descY_i.getLocalY() << ", " << descY_i.getLocalZ() << "]" << std::endl;

        auto nodeY = session->createComputeNode(descY_i);
        nodeY->bind("in_RGBA", imageViewsX[i]);
        nodeY->bind("out_RGBA", imageViewsY[i + 1]);
        nodeY->init();
        computeNodesY.push_back(nodeY);

        std::cout << std::endl;
    }
}


void ImagePyramid::record(ll::CommandBuffer& cmdBuffer) {

    for (auto i = 0u; i < levels; ++i) {

        cmdBuffer.run(*computeNodesX[i]);
        cmdBuffer.memoryBarrier();

        cmdBuffer.run(*computeNodesY[i]);
        cmdBuffer.memoryBarrier();
    }
}

void ImagePyramid::writeAllImages(std::shared_ptr<ll::Session> session, const fs::path &path) {

    auto i = 0u;
    for (auto imgViewX : imageViewsX) {
        
        auto outPath = fs::path {path};
        outPath.replace_filename("imgX_" + std::to_string(i) + ".bmp");

        writeImage(session, imgViewX->getImage(), outPath.string());
        ++ i;
    }

    i = 0u;
    for (auto imgViewY : imageViewsY) {

        auto outPath = fs::path{path};
        outPath.replace_filename("imgY_" + std::to_string(i) + ".bmp");

        writeImage(session, imgViewY->getImage(), outPath.string());
        ++ i;
    }
}

void ImagePyramid::writeImage(std::shared_ptr<ll::Session> session, std::shared_ptr<ll::Image> image, const std::string& filename) {

    const auto imageSize     = image->getSize();
    const auto currentLayout = image->getLayout();

    // create host visible memory
    const auto hostMemFlags = ll::MemoryPropertyFlagBits::HostVisible | ll::MemoryPropertyFlagBits::HostCoherent;
    auto hostMemory         = session->createMemory(hostMemFlags, imageSize);
    auto hostImage          = hostMemory->createBuffer(imageSize);

    auto cmdBuffer = session->createCommandBuffer();

    cmdBuffer->begin();
    cmdBuffer->changeImageLayout(*image, ll::ImageLayout::TransferSrcOptimal);
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


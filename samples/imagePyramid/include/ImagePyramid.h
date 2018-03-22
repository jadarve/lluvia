
#ifndef IMAGE_PYRAMID_H_
#define IMAGE_PYRAMID_H_

#include <lluvia/core.h>

#include <memory>
#include <vector>

class ImagePyramid {

public:
    ImagePyramid(const uint32_t levels);

    void setInputImage(std::shared_ptr<ll::Image> inputImage);
    void init(std::shared_ptr<ll::Session> session);


private:
    void initComputeNodes(std::shared_ptr<ll::Session> session);

    uint32_t levels;

    std::shared_ptr<ll::Image> inputImage;

    std::shared_ptr<ll::Memory> memory;

    std::vector<std::shared_ptr<ll::ImageView>> imageViewsX;
    std::vector<std::shared_ptr<ll::ImageView>> imageViewsY;


    std::vector<std::shared_ptr<ll::ComputeNode>> computeNodesX;
    std::vector<std::shared_ptr<ll::ComputeNode>> computeNodesY;


    std::shared_ptr<ll::Program> imageDownsampleXProgram;
    std::shared_ptr<ll::Program> imageDownsampleYProgram;
};

#endif // IMAGE_PYRAMID_H_
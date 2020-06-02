
#ifndef IMAGE_PYRAMID_H_
#define IMAGE_PYRAMID_H_

#include <lluvia/core.h>

#include <memory>
#include <vector>

#ifdef __linux__

// at least in Ubuntu 18.04 and clang 6 <filesystem> is still in experimental
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

#elif _WIN32

#include <filesystem>
namespace fs = std::filesystem;

#endif // OS switch


class ImagePyramid {

public:
    ImagePyramid(const uint32_t tLevels);

    void setInputImage(std::shared_ptr<ll::Image> tInputImage);
    void init(std::shared_ptr<ll::Session> session);
    void record(ll::CommandBuffer& cmdBuffer);

    void writeAllImages(std::shared_ptr<ll::Session> session, const fs::path &path);

private:
    void initComputeNodes(std::shared_ptr<ll::Session> session);

    void writeImage(std::shared_ptr<ll::Session> session, std::shared_ptr<ll::Image> image, const std::string& filename);

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

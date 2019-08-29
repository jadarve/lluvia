#ifndef IMAGE_PYRAMID_H_
#define IMAGE_PYRAMID_H_

#include <lluvia/core.h>

#include <map>
#include <memory>
#include <string>
#include <vector>


class ImagePyramid : public ll::ContainerNode {

public:
    ImagePyramid(const std::shared_ptr<ll::Session>& session);

    void record(ll::CommandBuffer& commandBuffer) const override;

    void writeAllImages(std::shared_ptr<ll::Session> session);
    void writeImage(std::shared_ptr<ll::Session> session, std::shared_ptr<ll::Image> image, const std::string& filename);

protected:
    void onInit() override;
};

#endif // IMAGE_PYRAMID_H_

#ifndef IMAGE_PYRAMID_H_
#define IMAGE_PYRAMID_H_

#include <lluvia/core.h>

#include <map>
#include <memory>
#include <string>


class ImagePyramid : public ll::ContainerNode {

public:
    ImagePyramid();

    std::shared_ptr<ll::Object> getPort(const std::string& name) const noexcept override = 0;

    void bind(const std::string& name, const std::shared_ptr<ll::Object>& obj) override = 0;

    void record(const vk::CommandBuffer& commandBuffer) const override = 0;

protected:
    void onInit() override = 0;

private:
    std::map<std::string, std::shared_ptr<ll::Object>> m_objects;
};

#endif // IMAGE_PYRAMID_H_

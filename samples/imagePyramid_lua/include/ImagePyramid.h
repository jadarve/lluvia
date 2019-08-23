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

    std::shared_ptr<ll::Object> getPort(const std::string& name) const noexcept override;

    void bind(const std::string& name, const std::shared_ptr<ll::Object>& obj) override;

    void record(ll::CommandBuffer& commandBuffer) const override;

protected:
    void onInit() override;

private:
    std::map<std::string, std::shared_ptr<ll::Object>> m_objects;

    std::vector<std::shared_ptr<ll::Node>> m_nodes;

    std::shared_ptr<ll::Session> m_session;
};

#endif // IMAGE_PYRAMID_H_

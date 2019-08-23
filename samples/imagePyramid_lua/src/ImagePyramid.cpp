
#include "ImagePyramid.h"


ImagePyramid::ImagePyramid(const std::shared_ptr<ll::Session>& session) :
    m_session {session} {

}


std::shared_ptr<ll::Object> ImagePyramid::getPort(const std::string& name) const noexcept {

    const auto it = m_objects.find(name);
    return it == m_objects.end()? nullptr : it->second;
}


void ImagePyramid::bind(__attribute__((unused)) const std::string& name,
    __attribute__((unused)) const std::shared_ptr<ll::Object>& obj) {

    // the ports of the node and the objects it contains are different stuff
}


void ImagePyramid::record(__attribute__((unused)) const vk::CommandBuffer& commandBuffer) const {

}

void ImagePyramid::onInit() {

    // I need a session to init stuff!
}

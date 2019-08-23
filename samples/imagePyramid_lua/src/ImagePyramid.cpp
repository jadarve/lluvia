
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


void ImagePyramid::record(ll::CommandBuffer& commandBuffer) const {

    for (const auto& node : m_nodes) {
        node->record(commandBuffer);
        commandBuffer.memoryBarrier();
    }
}

void ImagePyramid::onInit() {

    auto in_RGBA = getPort("in_RGBA");
    std::static_pointer_cast<ll::ImageView>(in_RGBA)->changeImageLayout(vk::ImageLayout::eGeneral);

    for (auto h = 0u; h < 4; ++h) {

        auto downX = m_session->createComputeNode("imageDownsampleX");
        auto downY = m_session->createComputeNode("imageDownsampleY");

        downX->bind("in_RGBA", in_RGBA);
        downX->init();

        downY->bind("in_RGBA", downX->getPort("out_RGBA"));
        downY->init();

        // the input to the next level will be the output of downY
        in_RGBA = downY->getPort("out_RGBA");

        m_nodes.push_back(downX);
        m_nodes.push_back(downY);
    }
}

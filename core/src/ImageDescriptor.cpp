#include "lluvia/core/ImageDescriptor.h"

namespace ll {

ImageDescriptor& ImageDescriptor::setChannelType(const ll::ChannelType type) noexcept {
    
    channelType = type;
    return *this;
}


ImageDescriptor& ImageDescriptor::setChannelCount(const uint32_t count) noexcept {

    channelCount = count;
    return *this;
}


ImageDescriptor& ImageDescriptor::setWidth(const uint32_t width) noexcept {
    this->width = width;
    return *this;
}


ImageDescriptor& ImageDescriptor::setHeight(const uint32_t height) noexcept {
    this->height = height;
    return *this;
}


ImageDescriptor& ImageDescriptor::setDepth(const uint32_t depth) noexcept {
    this->depth = depth;
    return *this;
}


ll::ChannelType ImageDescriptor::getChannelType() const noexcept {
    return channelType;
}


uint32_t ImageDescriptor::getChannelCount() const noexcept {
    return channelCount;
}


uint32_t ImageDescriptor::getWidth() const noexcept {
    return width;
}


uint32_t ImageDescriptor::getHeight() const noexcept {
    return height;
}


uint32_t ImageDescriptor::getDepth() const noexcept {
    return depth;
}



} // namespace ll
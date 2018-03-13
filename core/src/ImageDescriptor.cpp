#include "lluvia/core/ImageDescriptor.h"

namespace ll {


constexpr const char* STRING_CHANNEL_TYPE_UINT8   = "uint8";
constexpr const char* STRING_CHANNEL_TYPE_INT8    = "int8";
constexpr const char* STRING_CHANNEL_TYPE_UINT16  = "uint16";
constexpr const char* STRING_CHANNEL_TYPE_INT16   = "int16";
constexpr const char* STRING_CHANNEL_TYPE_FLOAT16 = "float16";
constexpr const char* STRING_CHANNEL_TYPE_UINT32  = "uint32";
constexpr const char* STRING_CHANNEL_TYPE_INT32   = "int32";
constexpr const char* STRING_CHANNEL_TYPE_FLOAT32 = "float32";
constexpr const char* STRING_CHANNEL_TYPE_UINT64  = "uint64";
constexpr const char* STRING_CHANNEL_TYPE_INT64   = "int64";
constexpr const char* STRING_CHANNEL_TYPE_FLOAT64 = "float64";


uint64_t getChannelTypeSize(ll::ChannelType type) {

    switch (type) {
        case ll::ChannelType::Uint8:
        case ll::ChannelType::Int8:
            return 1;

        case ll::ChannelType::Uint16:
        case ll::ChannelType::Int16:
        case ll::ChannelType::Float16:
            return 2;

        case ll::ChannelType::Uint32:
        case ll::ChannelType::Int32:
        case ll::ChannelType::Float32:
            return 4;

        case ll::ChannelType::Uint64:
        case ll::ChannelType::Int64:
        case ll::ChannelType::Float64:
            return 8;
    }
}


std::string channelTypeToString(ll::ChannelType type) {

    switch (type) {
        case ll::ChannelType::Uint8:   return STRING_CHANNEL_TYPE_UINT8;
        case ll::ChannelType::Int8:    return STRING_CHANNEL_TYPE_INT8;
        case ll::ChannelType::Uint16:  return STRING_CHANNEL_TYPE_UINT16;
        case ll::ChannelType::Int16:   return STRING_CHANNEL_TYPE_INT16;
        case ll::ChannelType::Float16: return STRING_CHANNEL_TYPE_FLOAT16;
        case ll::ChannelType::Uint32:  return STRING_CHANNEL_TYPE_UINT32;
        case ll::ChannelType::Int32:   return STRING_CHANNEL_TYPE_INT32;
        case ll::ChannelType::Float32: return STRING_CHANNEL_TYPE_FLOAT32;
        case ll::ChannelType::Uint64:  return STRING_CHANNEL_TYPE_UINT64;
        case ll::ChannelType::Int64:   return STRING_CHANNEL_TYPE_INT64;
        case ll::ChannelType::Float64: return STRING_CHANNEL_TYPE_FLOAT64;
    }
}


ll::ChannelType stringToChannelType(const std::string& str) {

    if (str == STRING_CHANNEL_TYPE_UINT8)   return ll::ChannelType::Uint8;
    if (str == STRING_CHANNEL_TYPE_INT8)    return ll::ChannelType::Int8;
    if (str == STRING_CHANNEL_TYPE_UINT16)  return ll::ChannelType::Uint16;
    if (str == STRING_CHANNEL_TYPE_INT16)   return ll::ChannelType::Int16;
    if (str == STRING_CHANNEL_TYPE_FLOAT16) return ll::ChannelType::Float16;
    if (str == STRING_CHANNEL_TYPE_UINT32)  return ll::ChannelType::Uint32;
    if (str == STRING_CHANNEL_TYPE_INT32)   return ll::ChannelType::Int32;
    if (str == STRING_CHANNEL_TYPE_FLOAT32) return ll::ChannelType::Float32;
    if (str == STRING_CHANNEL_TYPE_UINT64)  return ll::ChannelType::Uint64;
    if (str == STRING_CHANNEL_TYPE_INT64)   return ll::ChannelType::Int64;
    if (str == STRING_CHANNEL_TYPE_FLOAT64) return ll::ChannelType::Float64;

    throw std::runtime_error("unknown channel type string: " + str);
}


ImageDescriptor::ImageDescriptor(const uint32_t width,
                                 const uint32_t height,
                                 const uint32_t depth,
                                 const uint32_t channelCount,
                                 ll::ChannelType channelType):
    channelType  {channelType},
    channelCount {channelCount},
    width        {width},
    height       {height},
    depth        {depth} {

}


ImageDescriptor& ImageDescriptor::setChannelType(const ll::ChannelType type) noexcept {
    
    channelType = type;
    return *this;
}


ImageDescriptor& ImageDescriptor::setChannelCount(const uint32_t count) noexcept {

    assert(count > 0 && count <= 4);

    channelCount = count;
    return *this;
}


ImageDescriptor& ImageDescriptor::setWidth(const uint32_t width) noexcept {
    assert(width > 0);

    this->width = width;
    return *this;
}


ImageDescriptor& ImageDescriptor::setHeight(const uint32_t height) noexcept {
    assert(height > 0);

    this->height = height;
    return *this;
}


ImageDescriptor& ImageDescriptor::setDepth(const uint32_t depth) noexcept {
    assert(depth > 0);

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


uint64_t ImageDescriptor::getSize() const noexcept {

    return width*height*depth*channelCount*getChannelTypeSize(channelType);
}


vk::ImageType ImageDescriptor::getImageType() const noexcept {

    if (height == 1) return vk::ImageType::e1D;
    if (depth == 1)  return vk::ImageType::e2D;

    return vk::ImageType::e3D;
}

vk::Format ImageDescriptor::getFormat() const {

    switch (channelCount) {

        case 1:
            switch (channelType) {
                case ChannelType::Uint8:   return vk::Format::eR8Uint;
                case ChannelType::Int8:    return vk::Format::eR8Sint;
                case ChannelType::Uint16:  return vk::Format::eR16Uint;
                case ChannelType::Int16:   return vk::Format::eR16Sint;
                case ChannelType::Float16: return vk::Format::eR16Sfloat;
                case ChannelType::Uint32:  return vk::Format::eR32Uint;
                case ChannelType::Int32:   return vk::Format::eR32Sint;
                case ChannelType::Float32: return vk::Format::eR32Sfloat;
                case ChannelType::Uint64:  return vk::Format::eR64Uint;
                case ChannelType::Int64:   return vk::Format::eR64Sint;
                case ChannelType::Float64: return vk::Format::eR64Sfloat;
            }
        break;

        case 2:
            switch (channelType) {
                case ChannelType::Uint8:   return vk::Format::eR8G8Uint;
                case ChannelType::Int8:    return vk::Format::eR8G8Sint;
                case ChannelType::Uint16:  return vk::Format::eR16G16Uint;
                case ChannelType::Int16:   return vk::Format::eR16G16Sint;
                case ChannelType::Float16: return vk::Format::eR16G16Sfloat;
                case ChannelType::Uint32:  return vk::Format::eR32G32Uint;
                case ChannelType::Int32:   return vk::Format::eR32G32Sint;
                case ChannelType::Float32: return vk::Format::eR32G32Sfloat;
                case ChannelType::Uint64:  return vk::Format::eR64G64Uint;
                case ChannelType::Int64:   return vk::Format::eR64G64Sint;
                case ChannelType::Float64: return vk::Format::eR64G64Sfloat;
            }
        break;

        case 3:
            switch (channelType) {
                case ChannelType::Uint8:   return vk::Format::eR8G8B8Uint;
                case ChannelType::Int8:    return vk::Format::eR8G8B8Sint;
                case ChannelType::Uint16:  return vk::Format::eR16G16B16Uint;
                case ChannelType::Int16:   return vk::Format::eR16G16B16Sint;
                case ChannelType::Float16: return vk::Format::eR16G16B16Sfloat;
                case ChannelType::Uint32:  return vk::Format::eR32G32B32Uint;
                case ChannelType::Int32:   return vk::Format::eR32G32B32Sint;
                case ChannelType::Float32: return vk::Format::eR32G32B32Sfloat;
                case ChannelType::Uint64:  return vk::Format::eR64G64B64Uint;
                case ChannelType::Int64:   return vk::Format::eR64G64B64Sint;
                case ChannelType::Float64: return vk::Format::eR64G64B64Sfloat;
            }
        break;

        case 4:
            switch (channelType) {
                case ChannelType::Uint8:   return vk::Format::eR8G8B8A8Uint;
                case ChannelType::Int8:    return vk::Format::eR8G8B8A8Sint;
                case ChannelType::Uint16:  return vk::Format::eR16G16B16A16Uint;
                case ChannelType::Int16:   return vk::Format::eR16G16B16A16Sint;
                case ChannelType::Float16: return vk::Format::eR16G16B16A16Sfloat;
                case ChannelType::Uint32:  return vk::Format::eR32G32B32A32Uint;
                case ChannelType::Int32:   return vk::Format::eR32G32B32A32Sint;
                case ChannelType::Float32: return vk::Format::eR32G32B32A32Sfloat;
                case ChannelType::Uint64:  return vk::Format::eR64G64B64A64Uint;
                case ChannelType::Int64:   return vk::Format::eR64G64B64A64Sint;
                case ChannelType::Float64: return vk::Format::eR64G64B64A64Sfloat;
            }
        break;

        default:
        throw std::runtime_error("channel count must be between 1 and 4, got: " + std::to_string(channelCount));
    }
}

} // namespace ll

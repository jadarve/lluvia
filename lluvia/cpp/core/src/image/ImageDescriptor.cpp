/**
@file       ImageDescriptor.cpp
@brief      ImageDescriptor class and related enumerations.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#include "lluvia/core/image/ImageDescriptor.h"

namespace ll {

uint64_t getChannelTypeSize(ll::ChannelType type)
{

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

vk::Format getVulkanImageFormat(ll::ChannelCount channelCount, ll::ChannelType channelType) noexcept
{

    switch (channelCount) {

    case ll::ChannelCount::C1:
        switch (channelType) {
        case ChannelType::Uint8:
            return vk::Format::eR8Uint;
        case ChannelType::Int8:
            return vk::Format::eR8Sint;
        case ChannelType::Uint16:
            return vk::Format::eR16Uint;
        case ChannelType::Int16:
            return vk::Format::eR16Sint;
        case ChannelType::Float16:
            return vk::Format::eR16Sfloat;
        case ChannelType::Uint32:
            return vk::Format::eR32Uint;
        case ChannelType::Int32:
            return vk::Format::eR32Sint;
        case ChannelType::Float32:
            return vk::Format::eR32Sfloat;
        case ChannelType::Uint64:
            return vk::Format::eR64Uint;
        case ChannelType::Int64:
            return vk::Format::eR64Sint;
        case ChannelType::Float64:
            return vk::Format::eR64Sfloat;
        }
        break;

    case ll::ChannelCount::C2:
        switch (channelType) {
        case ChannelType::Uint8:
            return vk::Format::eR8G8Uint;
        case ChannelType::Int8:
            return vk::Format::eR8G8Sint;
        case ChannelType::Uint16:
            return vk::Format::eR16G16Uint;
        case ChannelType::Int16:
            return vk::Format::eR16G16Sint;
        case ChannelType::Float16:
            return vk::Format::eR16G16Sfloat;
        case ChannelType::Uint32:
            return vk::Format::eR32G32Uint;
        case ChannelType::Int32:
            return vk::Format::eR32G32Sint;
        case ChannelType::Float32:
            return vk::Format::eR32G32Sfloat;
        case ChannelType::Uint64:
            return vk::Format::eR64G64Uint;
        case ChannelType::Int64:
            return vk::Format::eR64G64Sint;
        case ChannelType::Float64:
            return vk::Format::eR64G64Sfloat;
        }
        break;

    case ll::ChannelCount::C3:
        switch (channelType) {
        case ChannelType::Uint8:
            return vk::Format::eR8G8B8Uint;
        case ChannelType::Int8:
            return vk::Format::eR8G8B8Sint;
        case ChannelType::Uint16:
            return vk::Format::eR16G16B16Uint;
        case ChannelType::Int16:
            return vk::Format::eR16G16B16Sint;
        case ChannelType::Float16:
            return vk::Format::eR16G16B16Sfloat;
        case ChannelType::Uint32:
            return vk::Format::eR32G32B32Uint;
        case ChannelType::Int32:
            return vk::Format::eR32G32B32Sint;
        case ChannelType::Float32:
            return vk::Format::eR32G32B32Sfloat;
        case ChannelType::Uint64:
            return vk::Format::eR64G64B64Uint;
        case ChannelType::Int64:
            return vk::Format::eR64G64B64Sint;
        case ChannelType::Float64:
            return vk::Format::eR64G64B64Sfloat;
        }
        break;

    case ll::ChannelCount::C4:
        switch (channelType) {
        case ChannelType::Uint8:
            return vk::Format::eR8G8B8A8Uint;
        case ChannelType::Int8:
            return vk::Format::eR8G8B8A8Sint;
        case ChannelType::Uint16:
            return vk::Format::eR16G16B16A16Uint;
        case ChannelType::Int16:
            return vk::Format::eR16G16B16A16Sint;
        case ChannelType::Float16:
            return vk::Format::eR16G16B16A16Sfloat;
        case ChannelType::Uint32:
            return vk::Format::eR32G32B32A32Uint;
        case ChannelType::Int32:
            return vk::Format::eR32G32B32A32Sint;
        case ChannelType::Float32:
            return vk::Format::eR32G32B32A32Sfloat;
        case ChannelType::Uint64:
            return vk::Format::eR64G64B64A64Uint;
        case ChannelType::Int64:
            return vk::Format::eR64G64B64A64Sint;
        case ChannelType::Float64:
            return vk::Format::eR64G64B64A64Sfloat;
        }
        break;
    }
}

ImageDescriptor::ImageDescriptor(const uint32_t depth,
    const uint32_t                              height,
    const uint32_t                              width,
    const ll::ChannelCount                      channelCount,
    const ll::ChannelType                       channelType,
    const ll::ImageUsageFlags                   usageFlags,
    const ll::ImageTiling                       tiling)
    : m_channelType {channelType}
    , m_channelCount {channelCount}
    , m_tiling {tiling}
    , m_usageFlags {usageFlags}
{

    setWidth(width);
    setHeight(height);
    setDepth(depth);
}

ImageDescriptor::ImageDescriptor(const uint32_t depth,
    const uint32_t                              height,
    const uint32_t                              width,
    const ll::ChannelCount                      channelCount,
    const ll::ChannelType                       channelType)
    : m_channelType {channelType}
    , m_channelCount {channelCount}
{

    setWidth(width);
    setHeight(height);
    setDepth(depth);
}

ImageDescriptor& ImageDescriptor::setChannelType(const ll::ChannelType type) noexcept
{

    m_channelType = type;
    return *this;
}

ImageDescriptor& ImageDescriptor::setChannelCount(const ll::ChannelCount count) noexcept
{

    m_channelCount = count;
    return *this;
}

ImageDescriptor& ImageDescriptor::setWidth(const uint32_t width) noexcept
{

    m_shape.x = width;
    return *this;
}

ImageDescriptor& ImageDescriptor::setHeight(const uint32_t height) noexcept
{

    m_shape.y = height;
    return *this;
}

ImageDescriptor& ImageDescriptor::setDepth(const uint32_t depth) noexcept
{

    m_shape.z = depth;
    return *this;
}

ImageDescriptor& ImageDescriptor::setShape(const ll::vec3ui& shape) noexcept
{

    m_shape = shape;
    return *this;
}

ImageDescriptor& ImageDescriptor::setUsageFlags(const ll::ImageUsageFlags flags) noexcept
{

    m_usageFlags = flags;
    return *this;
}

ImageDescriptor& ImageDescriptor::setTiling(const ll::ImageTiling tTiling) noexcept
{

    m_tiling = tTiling;
    return *this;
}

ll::ChannelType ImageDescriptor::getChannelType() const noexcept
{
    return m_channelType;
}

uint32_t ImageDescriptor::getWidth() const noexcept
{
    return m_shape.x;
}

uint32_t ImageDescriptor::getHeight() const noexcept
{
    return m_shape.y;
}

uint32_t ImageDescriptor::getDepth() const noexcept
{
    return m_shape.z;
}

uint64_t ImageDescriptor::getSize() const noexcept
{

    auto w = uint64_t {m_shape.x};
    auto h = uint64_t {m_shape.y};
    auto d = uint64_t {m_shape.z};
    auto c = static_cast<uint64_t>(m_channelCount);
    return w * h * d * c * getChannelTypeSize(m_channelType);
}

ll::vec3ui ImageDescriptor::getShape() const noexcept
{

    return m_shape;
}

vk::ImageType ImageDescriptor::getImageType() const noexcept
{

    if (getHeight() == 1 && getDepth() == 1) {
        return vk::ImageType::e1D;
    }

    if (getDepth() == 1) {
        return vk::ImageType::e2D;
    }

    return vk::ImageType::e3D;
}

vk::Format ImageDescriptor::getFormat() const noexcept
{

    return getVulkanImageFormat(m_channelCount, m_channelType);
}

ll::ImageUsageFlags ImageDescriptor::getUsageFlags() const noexcept
{
    return m_usageFlags;
}

ll::ImageTiling ImageDescriptor::getTiling() const noexcept
{
    return m_tiling;
}

ImageDescriptor& ImageDescriptor::setUsageFlagsUnsafe(const uint32_t flags) noexcept
{

    m_usageFlags = static_cast<ll::ImageUsageFlags>(flags);
    return *this;
}

uint32_t ImageDescriptor::getUsageFlagsUnsafe() const noexcept
{
    return static_cast<uint32_t>(m_usageFlags);
}

} // namespace ll

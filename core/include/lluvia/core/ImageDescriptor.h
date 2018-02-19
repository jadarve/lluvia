#ifndef LLUVIA_CORE_IMAGE_DESCRIPTOR_H_
#define LLUVIA_CORE_IMAGE_DESCRIPTOR_H_

#include <cstdint>

#include <vulkan/vulkan.hpp>


namespace ll {


enum class ChannelType : uint32_t {

    Uint8,
    Int8,

    Uint16,
    Int16,
    Float16,

    Uint32,
    Int32,
    Float32,

    Uint64,
    Int64,
    Float64
};


class ImageDescriptor {

public:
    ImageDescriptor()                                               = default;
    ImageDescriptor(const ImageDescriptor& descriptor)              = default;
    ImageDescriptor(ImageDescriptor&& descriptor)                   = default;

    ~ImageDescriptor()                                              = default;
    
    ImageDescriptor& operator = (const ImageDescriptor& descriptor) = default;
    ImageDescriptor& operator = (ImageDescriptor&& descriptor)      = default;

    ImageDescriptor& setChannelType(const ll::ChannelType type) noexcept;
    ImageDescriptor& setChannelCount(const uint32_t count)      noexcept;
    ImageDescriptor& setWidth(const uint32_t width)             noexcept;
    ImageDescriptor& setHeight(const uint32_t height)           noexcept;
    ImageDescriptor& setDepth(const uint32_t depth)             noexcept;

    ll::ChannelType getChannelType() const noexcept;
    uint32_t getChannelCount()       const noexcept;
    uint32_t getWidth()              const noexcept;
    uint32_t getHeight()             const noexcept;
    uint32_t getDepth()              const noexcept;

private:
    ll::ChannelType channelType;
    uint32_t channelCount;

    uint32_t width;
    uint32_t height;
    uint32_t depth;
};


} // namespace ll

#endif /* LLUVIA_CORE_IMAGE_DESCRIPTOR_H_ */

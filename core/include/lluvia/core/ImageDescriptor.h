#ifndef LLUVIA_CORE_IMAGE_DESCRIPTOR_H_
#define LLUVIA_CORE_IMAGE_DESCRIPTOR_H_

#include "lluvia/core/impl/enum_utils.h"

#include <cstdint>

#include <vulkan/vulkan.hpp>


namespace ll {


enum class ChannelType : uint32_t {
    Uint8   = 0,
    Int8    = 1,

    Uint16  = 2,
    Int16   = 3,
    Float16 = 4,

    Uint32  = 5,
    Int32   = 6,
    Float32 = 7,

    Uint64  = 8,
    Int64   = 9,
    Float64 = 10
};


namespace impl {

    constexpr const std::array<const char*, 11> ChannelTypeStrings {{
        "UINT8",
        "INT8",

        "UINT16",
        "INT16",
        "FLOAT16",

        "UINT32",
        "INT32",
        "FLOAT32",

        "UINT64",
        "INT64",
        "FLOAT64"
    }};

} // namespace impl


template<typename T = std::string>
inline T channelTypeToString(ll::ChannelType&& value) {
    return ll::impl::enumToString<ll::ChannelType, ll::impl::ChannelTypeStrings.size(), impl::ChannelTypeStrings>(std::forward<ll::ChannelType>(value));
}


template<typename T>
inline ll::ChannelType stringToChannelType(T&& stringValue) {
    return impl::stringToEnum<ll::ChannelType, T, ll::impl::ChannelTypeStrings.size(), impl::ChannelTypeStrings>(std::forward<T>(stringValue));
}


uint64_t getChannelTypeSize(ll::ChannelType type);


class ImageDescriptor {

public:
    ImageDescriptor()                                               = default;
    ImageDescriptor(const ImageDescriptor& descriptor)              = default;
    ImageDescriptor(ImageDescriptor&& descriptor)                   = default;

    ImageDescriptor(const uint32_t width,
                    const uint32_t height,
                    const uint32_t depth,
                    const uint32_t channelCount,
                    ll::ChannelType channelType);

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

    uint64_t getSize()               const noexcept;

    vk::ImageType getImageType() const noexcept;
    vk::Format getFormat() const;

private:
    ll::ChannelType channelType  {ll::ChannelType::Uint8};
    uint32_t channelCount        {1};

    uint32_t width  {1};
    uint32_t height {1};
    uint32_t depth  {1};
};


} // namespace ll

#endif /* LLUVIA_CORE_IMAGE_DESCRIPTOR_H_ */

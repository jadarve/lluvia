/**
@file       ImageDescriptor.h
@brief      ImageDescriptor class and related enumerations.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_IMAGE_DESCRIPTOR_H_
#define LLUVIA_CORE_IMAGE_DESCRIPTOR_H_

#include "lluvia/core/impl/enum_utils.h"

#include <array>
#include <cstdint>
#include <string>
#include <tuple>

#include <vulkan/vulkan.hpp>


namespace ll {


/**
@brief      Supported image channel types.

@sa         ll::impl::ChannelTypeStrings string values for this enumeration.
*/
enum class ChannelType : uint32_t {
    Uint8   = 0,    /**< 8-bit unsigned int. */
    Int8    = 1,    /**< 8-bit signed int. */

    Uint16  = 2,    /**< 16-bit unsigned int. */
    Int16   = 3,    /**< 16-bit signed int. */
    Float16 = 4,    /**< 16-bit floating point. */

    Uint32  = 5,    /**< 32-bit unsigned int. */
    Int32   = 6,    /**< 32-bit signed int. */
    Float32 = 7,    /**< 32-bit floating point. */

    Uint64  = 8,    /**< 64-bit unsigned int. */
    Int64   = 9,    /**< 64-bit signed int. */
    Float64 = 10    /**< 64-bit floating point. */
};


namespace impl {

    /**
    @brief Channel type string values used for converting ll::ChannelType to std::string and vice-versa.

    @sa ll::ChannelType enum values for this array.
    */
    constexpr const std::array<std::tuple<const char*, ll::ChannelType>, 11> ChannelTypeStrings {{
        std::make_tuple("UINT8"   , ll::ChannelType::Uint8),
        std::make_tuple("INT8"    , ll::ChannelType::Int8),

        std::make_tuple("UINT16"  , ll::ChannelType::Uint16),
        std::make_tuple("INT16"   , ll::ChannelType::Int16),
        std::make_tuple("FLOAT16" , ll::ChannelType::Float16),

        std::make_tuple("UINT32"  , ll::ChannelType::Uint32),
        std::make_tuple("INT32"   , ll::ChannelType::Int32),
        std::make_tuple("FLOAT32" , ll::ChannelType::Float32),

        std::make_tuple("UINT64"  , ll::ChannelType::Uint64),
        std::make_tuple("INT64"   , ll::ChannelType::Int64),
        std::make_tuple("FLOAT64" , ll::ChannelType::Float64),
    }};

} // namespace impl


/**
@brief      Converts from ll::ChannelType enum value to std::string.

@param[in]  value      

@tparam     T          function return type. Defaults to std::string.

@return     Returns the corresponding `std::string` in ll::impl::ChannelTypeStrings for the enum value.
*/
template<typename T = std::string>
inline T channelTypeToString(ll::ChannelType&& value) noexcept {
    return ll::impl::enumToString<ll::ChannelType, ll::impl::ChannelTypeStrings.size(), impl::ChannelTypeStrings>(std::forward<ll::ChannelType>(value));
}


/**
@brief      Converts from a string-like object to ll::ChannelType enum.

This function can be used either with string literals, const char* or `std::string` objects.
\p stringValue parameter is compared against the values in ll::impl::ChannelTypeStrings and the
corresponding enum value is returned. The comparison is case sensitive.

@param[in]  stringValue  string-like parameter. String literals and `std::string` objects are allowed.

@tparam     T            \p stringValue type. \p T must satisfies `std::is_convertible<T, std::string>()`

@return     ll::ChannelType value corresponding to stringValue
 
@throws std::out_of_range if \p stringValue is not found in ll::ObjectTypeStrings.
*/
template<typename T>
inline ll::ChannelType stringToChannelType(T&& stringValue) {
    return impl::stringToEnum<ll::ChannelType, T, ll::impl::ChannelTypeStrings.size(), impl::ChannelTypeStrings>(std::forward<T>(stringValue));
}


/**
@brief      Gets size in bytes of a given channel type.

@param[in]  type  The channel type.

@return     The channel type size in bytes.
*/
uint64_t getChannelTypeSize(ll::ChannelType type);


/**
@brief      Stores all relevant information for creating ll::Image objects.

The following code creates a descriptor for an 4-channel image with 640x480 pixel resolution
and 8-bit unsigned int channel type.

@code
    auto desc = ll::ImageDescriptor{}
                    .setWidth(640)
                    .setHeight(480)
                    .setChannelType(ll::ChannelType::Uint8)
                    .setChannelCount(4);
@endcode
*/
class ImageDescriptor {

public:
    ImageDescriptor()                                               = default;
    ImageDescriptor(const ImageDescriptor& descriptor)              = default;
    ImageDescriptor(ImageDescriptor&& descriptor)                   = default;


    /**
    @brief      Constructs the object.
    
    @param[in]  width         The width. It must be greater than zero.
    @param[in]  height        The height. It must be greater than zero.
    @param[in]  depth         The depth. It must be greater than zero.
    @param[in]  channelCount  The channel count. It must be between 1 and 4.
    @param[in]  channelType   The channel type.
    */
    ImageDescriptor(const uint32_t width,
                    const uint32_t height,
                    const uint32_t depth,
                    const uint32_t channelCount,
                    ll::ChannelType channelType);

    ~ImageDescriptor()                                              = default;
    
    ImageDescriptor& operator = (const ImageDescriptor& descriptor) = default;
    ImageDescriptor& operator = (ImageDescriptor&& descriptor)      = default;


    /**
    @brief      Sets the channel type.
    
    @param[in]  type  The channel type.
    
    @return     A reference to this object.
    */
    ImageDescriptor& setChannelType(const ll::ChannelType type) noexcept;


    /**
    @brief      Sets the channel count.
    
    @param[in]  count  The count. It must be between 1 and 4.
    
    @return     A reference to this object.
    */
    ImageDescriptor& setChannelCount(const uint32_t count)      noexcept;


    /**
    @brief      Sets the width.
    
    @param[in]  width  The width. It must be greater than zero.
    
    @return     A reference to this object.
    */
    ImageDescriptor& setWidth(const uint32_t width)             noexcept;


    /**
    @brief      Sets the height.
    
    @param[in]  height  The height. It must be greater than zero.
    
    @return     A reference to this object.
    */
    ImageDescriptor& setHeight(const uint32_t height)           noexcept;


    /**
    @brief      Sets the depth.
    
    @param[in]  depth  The depth. It must be greater than zero.
                       
    @return     A reference to this object.
    */
    ImageDescriptor& setDepth(const uint32_t depth)             noexcept;


    /**
    @brief      Gets the channel type.
    
    @return     The channel type.
    */
    ll::ChannelType getChannelType() const noexcept;


    /**
    @brief      Gets the channel count.
    
    @return     The channel count. Number between 1 and 4.
    */
    uint32_t getChannelCount()       const noexcept;


    /**
    @brief      Gets the image width in pixels.
    
    @return     The image width in pixels.
    */
    uint32_t getWidth()              const noexcept;


    /**
    @brief      Gets the image height in pixels.
    
    @return     The image height in pixels.
    */
    uint32_t getHeight()             const noexcept;


    /**
    @brief      Gets the image depth in pixels.
    
    @return     The image depth in pixels.
    */
    uint32_t getDepth()              const noexcept;

    /**
    @brief      Gets the size in bytes required to store the image.

    This value does not include any row padding needed to align the one
    row row of data to the required size by the device.
    
    @return     The size of the image in bytes.
    */
    uint64_t getSize()               const noexcept;

    /**
    @brief      Gets the Vulkan image type.

    It is implemented as:
    @code
    if (getHeight() == 1) return vk::ImageType::e1D;
    if (getDepth()  == 1) return vk::ImageType::e2D;

    return vk::ImageType::e3D;
    @endcode
    
    @return     The image type.
    */
    vk::ImageType getImageType() const noexcept;

    /**
    @brief      Gets the corresponding Vulkan image format.

    The format is computed using the channel count and the channel type.

    See @VULKAN_DOC#_vkformat_3.
    
    @return     The format.
    */
    vk::Format getFormat() const noexcept;

private:
    ll::ChannelType channelType  {ll::ChannelType::Uint8};
    uint32_t channelCount        {1};

    uint32_t width  {1};
    uint32_t height {1};
    uint32_t depth  {1};
};


} // namespace ll

#endif /* LLUVIA_CORE_IMAGE_DESCRIPTOR_H_ */

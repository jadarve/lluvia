/**
@file       ImageDescriptor.h
@brief      ImageDescriptor class and related enumerations.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_IMAGE_IMAGE_DESCRIPTOR_H_
#define LLUVIA_CORE_IMAGE_IMAGE_DESCRIPTOR_H_

#include "lluvia/core/error.h"
#include "lluvia/core/image/ImageTiling.h"
#include "lluvia/core/image/ImageUsageFlags.h"
#include "lluvia/core/impl/enum_utils.h"
#include "lluvia/core/types.h"

#include <array>
#include <cstdint>
#include <string>
#include <tuple>
#include <type_traits>

#include "lluvia/core/vulkan/vulkan.hpp"


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


/**
@brief      Supported image channel count.
*/
enum class ChannelCount : uint32_t {
    C1 = 1,
    C2 = 2,
    C3 = 3,
    C4 = 4
};


namespace impl {

    /**
    @brief Channel type string values used for converting ll::ChannelType to std::string and vice-versa.

    @sa ll::ChannelType enum values for this array.
    */
    constexpr const std::array<std::tuple<const char*, ll::ChannelType>, 11> ChannelTypeStrings {{
        std::make_tuple("Uint8"   , ll::ChannelType::Uint8),
        std::make_tuple("Int8"    , ll::ChannelType::Int8),

        std::make_tuple("Uint16"  , ll::ChannelType::Uint16),
        std::make_tuple("Int16"   , ll::ChannelType::Int16),
        std::make_tuple("Float16" , ll::ChannelType::Float16),

        std::make_tuple("Uint32"  , ll::ChannelType::Uint32),
        std::make_tuple("Int32"   , ll::ChannelType::Int32),
        std::make_tuple("Float32" , ll::ChannelType::Float32),

        std::make_tuple("Uint64"  , ll::ChannelType::Uint64),
        std::make_tuple("Int64"   , ll::ChannelType::Int64),
        std::make_tuple("Float64" , ll::ChannelType::Float64),
    }};


    /**
    @brief Channel type string values used for converting ll::ChannelType to std::string and vice-versa.

    @sa ll::ChannelType enum values for this array.
    */
    constexpr const std::array<std::tuple<const char*, ll::ChannelCount>, 4> ChannelCountStrings {{
        std::make_tuple("C1", ll::ChannelCount::C1),
        std::make_tuple("C2", ll::ChannelCount::C2),
        std::make_tuple("C3", ll::ChannelCount::C3),
        std::make_tuple("C4", ll::ChannelCount::C4)
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

@return     ll::ChannelType value corresponding to stringValue.
 
@throws std::out_of_range if \p stringValue is not found in ll::impl::ObjectTypeStrings.
*/
template<typename T>
inline ll::ChannelType stringToChannelType(T&& stringValue) {
    return impl::stringToEnum<ll::ChannelType, T, ll::impl::ChannelTypeStrings.size(), impl::ChannelTypeStrings>(std::forward<T>(stringValue));
}


template<typename T=uint32_t>
ll::ChannelCount castChannelCount(T c) {
    static_assert(std::is_integral<T>::value, "T must be an integral type.");

    const auto casted = static_cast<std::underlying_type<ll::ChannelCount>::type>(c);

    constexpr const auto minValue = static_cast<std::underlying_type<ll::ChannelCount>::type>(ll::ChannelCount::C1);
    constexpr const auto maxValue = static_cast<std::underlying_type<ll::ChannelCount>::type>(ll::ChannelCount::C4);

    ll::throwSystemErrorIf(
        casted < minValue || casted > maxValue,
        ll::ErrorCode::BadEnumCasting,
        "Error casting integral value " + std::to_string(c) + " to ll::ChannelCount. " +
        "Expected value must be within range [" + std::to_string(minValue) + ", " + std::to_string(maxValue) + "].");

    return static_cast<ll::ChannelCount>(c);
}


/**
@brief      Gets size in bytes of a given channel type.

@param[in]  type  The channel type.

@return     The channel type size in bytes.
*/
uint64_t getChannelTypeSize(ll::ChannelType type);


/**
@brief      Gets the vulkan image format for a given channel count and type.

@param[in]  channels  The channel count
@param[in]  type      The channel type

@return     The vulkan image format.
*/
vk::Format getVulkanImageFormat(ll::ChannelCount channelCount, ll::ChannelType channelType) noexcept;


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
    
    @param[in]  depth          The depth. It must be greater than zero.
    @param[in]  height         The height. It must be greater than zero.
    @param[in]  width          The width. It must be greater than zero.
    @param[in]  channelCount   The channel count. It must be between 1 and 4.
    @param[in]  channelType    The channel type.
    */
    ImageDescriptor(const uint32_t depth,
                    const uint32_t height,
                    const uint32_t width,
                    const ll::ChannelCount channelCount,
                    const ll::ChannelType channelType);

    /**
    @brief      Constructs the object.
    
    @param[in]  depth          The depth. It must be greater than zero.
    @param[in]  height         The height. It must be greater than zero.
    @param[in]  width          The width. It must be greater than zero.
    @param[in]  channelCount   The channel count. It must be between 1 and 4.
    @param[in]  channelType    The channel type.
    @param[in]  usageFlags     The usage flags. See @VULKAN_DOC#VkImageUsageFlagBits.
    @param[in]  tiling         The image tiling. See @VULKAN_DOC#VkImageTiling.
    */
    ImageDescriptor(const uint32_t depth,
                    const uint32_t height,
                    const uint32_t width,
                    const ll::ChannelCount channelCount,
                    const ll::ChannelType channelType,
                    const ll::ImageUsageFlags usageFlags,
                    const ll::ImageTiling tiling);

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
    ImageDescriptor& setChannelCount(const ll::ChannelCount count)      noexcept;


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
    @brief      Sets the image shape.
    
    @param[in]  shape  The shape. The components of this vector
        must be interpreted as:
        
            x : width
            y : height
            z : depth
    
    @return     A reference to this object.
    */
    ImageDescriptor& setShape(const ll::vec3ui& shape)          noexcept;


    /**
    @brief      Sets the usage flags.
    
    @return     A reference to this object.
    */
    ImageDescriptor& setUsageFlags(const ll::ImageUsageFlags flags) noexcept;


    /**
    @brief      Sets the usage flags from an integer type.

    Please do not use this method. It's for internal use only.
    
    @param[in]  flags  The flags
    
    @return     A reference to this object
    */
    ImageDescriptor& setUsageFlagsUnsafe(const uint32_t flags) noexcept;


    /**
    @brief      Sets the vulkan image tiling.
    
    @param[in]  tTiling  The tiling
    
    @return     A reference to this object.
    */
    ImageDescriptor& setTiling(const ll::ImageTiling tTiling) noexcept;


    /**
    @brief      Gets the channel type.
    
    @return     The channel type.
    */
    ll::ChannelType getChannelType() const noexcept;


    /**
    @brief      Gets the channel count.
    
    @tparam     T     Type of the return value. Defaults to ll::ChannelCount.
    
    @return     The channel count.
    */
    template<typename T=ll::ChannelCount>
    T getChannelCount() const noexcept {
        return static_cast<T>(m_channelCount);
    }


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

    This value does not include any row padding needed to align one
    row of data to the required size by the device.
    
    @return     The size of the image in bytes.
    */
    uint64_t getSize()               const noexcept;

    /**
    @brief      Gets the shape of the image.
    
    The vec3ui object returned must be interpreted as follows:

        x : width
        y : height
        z : depth

    @return     The shape.
    */
    ll::vec3ui getShape() const noexcept;

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

    /**
    @brief      Gets the Vulkan image usage flags.
    
    See @VULKAN_DOC#VkImageUsageFlagBits
    for more information.

    @return     The usage flags.
    */
    ll::ImageUsageFlags getUsageFlags() const noexcept;

    /**
    @brief      Gets the vulkan image tiling.
    
    @return     The image tiling.
    */
    ll::ImageTiling getTiling() const noexcept;

    /**
    @brief      Gets the usage flags casted to an integer type.

    Please do not use this method. It's for internal use only.
    
    @return     The usage flags unsafe.
    */
    uint32_t getUsageFlagsUnsafe() const noexcept;

private:
    ll::ChannelType  m_channelType   {ll::ChannelType::Uint8};
    ll::ChannelCount m_channelCount  {ll::ChannelCount::C1};

    // dimensions along each axis
    // x : width
    // y : height
    // z : depth
    ll::vec3ui m_shape               {1, 1, 1};

    ll::ImageTiling m_tiling         {ll::ImageTiling::Optimal};
    ll::ImageUsageFlags m_usageFlags {ll::ImageUsageFlagBits::Storage |
                                      ll::ImageUsageFlagBits::Sampled |
                                      ll::ImageUsageFlagBits::TransferSrc |
                                      ll::ImageUsageFlagBits::TransferDst};
};


} // namespace ll

#endif /* LLUVIA_CORE_IMAGE_IMAGE_DESCRIPTOR_H_ */

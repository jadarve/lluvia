/**
@file       ImageViewDescriptor.h
@brief      ImageViewDescriptor class and related enumerations and methods.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_IMAGE_IMAGE_VIEW_DESCRIPTOR_H_
#define LLUVIA_CORE_IMAGE_IMAGE_VIEW_DESCRIPTOR_H_

#include "lluvia/core/impl/enum_utils.h"

#include <array>
#include <cstdint>
#include <tuple>
#include <string>

#include "lluvia/core/vulkan/vulkan.hpp"


namespace ll {

class ImageView;


/**
@brief      Image axis enumeration.
*/
enum class ImageAxis : uint32_t {
    U = 0,  /**< value for U or X axis. */
    V = 1,  /**< value for V or Y axis. */
    W = 2   /**< value for W or Z axis. */
};


/**
@brief      Image filter modes.

@sa         ll::impl::ImageFilterModeStrings string values for this enum.
*/
enum class ImageFilterMode : uint32_t {
    Nearest = 0,    /**< Nearest filter. */
    Linear  = 1     /**< Linear filter. */
};


/**
@brief      Image address mode.

Defines the behaviour when accessing image values outside of its
boundaries.

See @VULKAN_DOC#_vksampleraddressmode_3 for more information on the
aviable address modes.

@sa         ll::impl::ImageAddressModeStrings string values for this enum.
*/
enum class ImageAddressMode : uint32_t {
    Repeat            = 0,
    MirroredRepeat    = 1,
    ClampToEdge       = 2,
    ClampToBorder     = 3,
    MirrorClampToEdge = 4
};


namespace impl {

    /**
    @brief Image axis strings used for converting ll::ImageAxis to std::string and vice-versa.

    @sa         ll::ImageAxis enum values for this array.
    */
    constexpr const std::array<std::tuple<const char*, ll::ImageAxis>, 3> ImageAxisStrings {{
        std::make_tuple("U" , ll::ImageAxis::U),
        std::make_tuple("V"  , ll::ImageAxis::V),
        std::make_tuple("W"  , ll::ImageAxis::W),
    }};

    /**
    @brief Image filter mode strings used for converting ll::ImageFilterMode to std::string and vice-versa.

    @sa         ll::ImageFilterMode enum values for this array.
    */
    constexpr const std::array<std::tuple<const char*, ll::ImageFilterMode>, 2> ImageFilterModeStrings {{
        std::make_tuple("Nearest" , ll::ImageFilterMode::Nearest),
        std::make_tuple("Linear"  , ll::ImageFilterMode::Linear),
    }};


    /**
    @brief Image filter mode strings used for converting ll::ImageAddressMode to std::string and vice-versa.

    @sa         ll::ImageAddressMode enum values for this array.
    */
    constexpr const std::array<std::tuple<const char*, ll::ImageAddressMode>, 5> ImageAddressModeStrings {{
        std::make_tuple("Repeat"            , ll::ImageAddressMode::Repeat),
        std::make_tuple("MirroredRepeat"    , ll::ImageAddressMode::MirroredRepeat),
        std::make_tuple("ClampToEdge"       , ll::ImageAddressMode::ClampToEdge),
        std::make_tuple("ClampToBorder"     , ll::ImageAddressMode::ClampToBorder),
        std::make_tuple("MirrorClampToEdge" , ll::ImageAddressMode::MirrorClampToEdge),
    }};

} // namespace impl



/**
@brief      Converts from ll::ImageFilterMode enum value to std::string.

@param[in]  value

@tparam     T          function return type. Defaults to std::string.

@return     Returns the corresponding `std::string` in ll::impl::ImageFilterModeStrings for the enum value.
*/
template<typename T = std::string>
inline T imageFilterModeToString(ll::ImageFilterMode&& value) noexcept {
    return ll::impl::enumToString<ll::ImageFilterMode, impl::ImageFilterModeStrings.size(), impl::ImageFilterModeStrings>(std::forward<ll::ImageFilterMode>(value));
}


/**
@brief      Converts from a string-like object to ll::ImageFilterMode enum.

This function can be used either with string literals, `const char*` or `std::string` objects.
\p stringValue parameter is compared against the values in ll::impl::ImageFilterModeStrings and the
corresponding enum value is returned. The comparison is case sensitive.

@param[in]  stringValue  string-like parameter. String literals and std::string objects are allowed.

@tparam     T          \p stringValue type. \p T must satisfies `std::is_convertible<T, std::string>()`

@return     ll::ImageFilterMode value corresponding to \p stringValue

@throws     std::out_of_range if \p stringValue is not found in ll::impl::ImageFilterModeStrings.
*/
template<typename T>
inline ll::ImageFilterMode stringToImageFilterMode(T&& stringValue) {
    return impl::stringToEnum<ll::ImageFilterMode, T, ll::impl::ImageFilterModeStrings.size(), impl::ImageFilterModeStrings>(std::forward<T>(stringValue));
}


/**
@brief      Converts from ll::ImageAddressMode enum value to std::string.

@param[in]  value

@tparam     T          function return type. Defaults to std::string.

@return     Returns the corresponding `std::string` in ll::impl::ImageAddressModeStrings for the enum value.
*/
template<typename T = std::string>
inline T imageAddressModeToString(ll::ImageAddressMode&& value) noexcept {
    return impl::enumToString<ll::ImageAddressMode, ll::impl::ImageAddressModeStrings.size(), ll::impl::ImageAddressModeStrings>(std::forward<ll::ImageAddressMode>(value));
}


/**
@brief      Converts from a string-like object to ll::ImageAddressMode enum.

This function can be used either with string literals, `const char*` or `std::string` objects.
\p stringValue parameter is compared against the values in ll::impl::ImageAddressModeStrings and the
corresponding enum value is returned. The comparison is case sensitive.

@param[in]  stringValue  string-like parameter. String literals and std::string objects are allowed.

@tparam     T          \p stringValue type. \p T must satisfies `std::is_convertible<T, std::string>()`

@return     ll::ImageAddressMode value corresponding to \p stringValue

@throws     std::out_of_range if \p stringValue is not found in ll::impl::ImageAddressModeStrings.
*/
template<typename T>
inline ll::ImageAddressMode stringToImageAddressMode(T&& stringValue) {
    return impl::stringToEnum<ll::ImageAddressMode, T, ll::impl::ImageAddressModeStrings.size(), impl::ImageAddressModeStrings>(std::forward<T>(stringValue));
}


/**
@brief      Descriptor for ll::ImageView objects.

The following code creates a descriptor for a sampled image using nearest
filtering and unnormalized coordinates. The repeat address mode is used
for pixels outside the image boundaries.

@code
    auto imgViewDesc = ll::ImageViewDescriptor {}
                        .setNormalizedCoordinates(false)
                        .setIsSampled(true)
                        .setAddressMode(ll::ImageAddressMode::Repeat)
                        .setFilterMode(ll::ImageFilterMode::Nearest);
@endcode
*/
class ImageViewDescriptor {

public:
    ImageViewDescriptor()                                                   = default;
    ImageViewDescriptor(const ImageViewDescriptor& descriptor)              = default;
    ImageViewDescriptor(ImageViewDescriptor&& descriptor)                   = default;

    ImageViewDescriptor(const ll::ImageAddressMode addressmode,
                        const ll::ImageFilterMode filterMode,
                        const bool normalizedCoordinates,
                        const bool isSampled);

    ~ImageViewDescriptor()                                                  = default;
    
    ImageViewDescriptor& operator = (const ImageViewDescriptor& descriptor) = default;
    ImageViewDescriptor& operator = (ImageViewDescriptor&& descriptor)      = default;


    /**
    @brief      Sets the filtering mode.
    
    @param[in]  filterMode  The filter mode
    
    @return     A reference to this object.
    */
    ImageViewDescriptor& setFilterMode(ll::ImageFilterMode filterMode) noexcept;


    /**
    @brief      Gets the filter mode.
    
    @return     The filter mode.
    */
    ll::ImageFilterMode getFilterMode() const noexcept;


    /**
    @brief      Sets the address mode to all image axes.
    
    @param[in]  addressMode  The address mode
    
    @return     A reference to this object.
    */
    ImageViewDescriptor& setAddressMode(ll::ImageAddressMode addressMode) noexcept;


    /**
    @brief      Sets the address mode to a given image axis.
    
    @param[in]  axis           The axis
    @param[in]  addressMode   The address mode
    
    @return     A reference to this object.
    */
    ImageViewDescriptor& setAddressMode(ll::ImageAxis axis, ll::ImageAddressMode addressMode) noexcept;


    /**
    @brief      Gets the address mode for the U axis.
    
    @return     The address mode.
    */
    ll::ImageAddressMode getAddressModeU() const noexcept;


    /**
    @brief      Gets the address mode for the V axis.
    
    @return     The address mode.
    */
    ll::ImageAddressMode getAddressModeV() const noexcept;


    /**
    @brief      Gets the address mode for the W axis.
    
    @return     The address mode.
    */
    ll::ImageAddressMode getAddressModeW() const noexcept;


    /**
    @brief      Sets whether or not the image view will use normalized coordinates.
    
    @param[in]  normalizedCoordinates  The normalized coordinates.
    
    @return     A reference to this object.
    */
    ImageViewDescriptor& setNormalizedCoordinates(bool normalizedCoordinates) noexcept;


    /**
    @brief      Determines if normalized coordinates are used for this image view.
    
    @return     True if normalized coordinates, False otherwise.
    */
    bool isNormalizedCoordinates() const noexcept;


    /**
    @brief      Sets whether or not the image view will be sampled.
    
    @param[in]  isSampled  Indicates if sampled
    
    @return     A reference to this object.
    */
    ImageViewDescriptor& setIsSampled(bool isSampled) noexcept;


    /**
    @brief      Determines if the image view is sampled.
    
    @return     True if sampled, False otherwise.
    */
    bool isSampled() const noexcept;


    /**
    @brief      Return the Vulkan sampler creation info filled from this object.
    
    See @VULKAN_DOC#VkSamplerCreateInfo

    @return     The Vulkan sampler create info.
    */
    vk::SamplerCreateInfo getVkSamplerCreateInfo() const noexcept;


private:
    ll::ImageFilterMode m_filterMode {ll::ImageFilterMode::Nearest};

    /**
     * Address mode for U, V, W axes
     */
    std::array<ll::ImageAddressMode, 3> m_addressMode {{ll::ImageAddressMode::Repeat,
                                                        ll::ImageAddressMode::Repeat,
                                                        ll::ImageAddressMode::Repeat}};

    bool m_normalizedCoordinates {false};
    bool m_isSampled             {false};

};


} // namespace ll

#endif /* LLUVIA_CORE_IMAGE_IMAGE_VIEW_DESCRIPTOR_H_ */

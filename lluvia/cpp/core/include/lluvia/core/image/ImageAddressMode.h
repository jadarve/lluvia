/**
@file       ImageAxis.h
@brief      ImageAxis enum.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_IMAGE_IMAGE_ADDRESS_MODE_H_
#define LLUVIA_CORE_IMAGE_IMAGE_ADDRESS_MODE_H_

#include "lluvia/core/enums/enums.h"

namespace ll {

/**
@brief      Image address mode.

Defines the behaviour when accessing image values outside of its
boundaries.

See @VULKAN_DOC#_vksampleraddressmode_3 for more information on the
aviable address modes.

@sa         ll::impl::ImageAddressModeStrings string values for this enum.
*/
enum class ImageAddressMode : uint32_t {
    Repeat = 0,
    MirroredRepeat = 1,
    ClampToEdge = 2,
    ClampToBorder = 3,
    MirrorClampToEdge = 4
};

namespace impl {

    /**
    @brief Image filter mode strings used for converting ll::ImageAddressMode to std::string and vice-versa.

    @sa         ll::ImageAddressMode enum values for this array.
    */
    constexpr const std::array<std::tuple<const char*, ll::ImageAddressMode>, 5> ImageAddressModeStrings { {
        std::make_tuple("Repeat", ll::ImageAddressMode::Repeat),
        std::make_tuple("MirroredRepeat", ll::ImageAddressMode::MirroredRepeat),
        std::make_tuple("ClampToEdge", ll::ImageAddressMode::ClampToEdge),
        std::make_tuple("ClampToBorder", ll::ImageAddressMode::ClampToBorder),
        std::make_tuple("MirrorClampToEdge", ll::ImageAddressMode::MirrorClampToEdge),
    } };

} // namespace impl

/**
@brief      Converts from ll::ImageAddressMode enum value to std::string.

@param[in]  value

@tparam     T          function return type. Defaults to std::string.

@return     Returns the corresponding `std::string` in ll::impl::ImageAddressModeStrings for the enum value.
*/
template <typename T = std::string>
inline T imageAddressModeToString(ll::ImageAddressMode&& value) noexcept
{
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
template <typename T>
inline ll::ImageAddressMode stringToImageAddressMode(T&& stringValue)
{
    return impl::stringToEnum<ll::ImageAddressMode, T, ll::impl::ImageAddressModeStrings.size(), impl::ImageAddressModeStrings>(std::forward<T>(stringValue));
}

} // namespace ll

#endif // LLUVIA_CORE_IMAGE_IMAGE_ADDRESS_MODE_H_

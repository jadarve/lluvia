/**
@file       ImageFilterMode.h
@brief      ImageFilterMode enum.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_IMAGE_IMAGE_FILTER_MODE_H_
#define LLUVIA_CORE_IMAGE_IMAGE_FILTER_MODE_H_

#include "lluvia/core/enums/enums.h"

namespace ll {

/**
@brief      Image filter modes.

@sa         ll::impl::ImageFilterModeStrings string values for this enum.
*/
enum class ImageFilterMode : ll::enum_t {
    Nearest = 0, /**< Nearest filter. */
    Linear = 1 /**< Linear filter. */
};

namespace impl {

    /**
    @brief Image filter mode strings used for converting ll::ImageFilterMode to std::string and vice-versa.

    @sa         ll::ImageFilterMode enum values for this array.
    */
    constexpr const std::array<std::tuple<const char*, ll::ImageFilterMode>, 2> ImageFilterModeStrings { {
        std::make_tuple("Nearest", ll::ImageFilterMode::Nearest),
        std::make_tuple("Linear", ll::ImageFilterMode::Linear),
    } };

} // namespace impl

/**
@brief      Converts from ll::ImageFilterMode enum value to std::string.

@param[in]  value

@tparam     T          function return type. Defaults to std::string.

@return     Returns the corresponding `std::string` in ll::impl::ImageFilterModeStrings for the enum value.
*/
template <typename T = std::string>
inline T imageFilterModeToString(ll::ImageFilterMode&& value) noexcept
{
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
template <typename T>
inline ll::ImageFilterMode stringToImageFilterMode(T&& stringValue)
{
    return impl::stringToEnum<ll::ImageFilterMode, T, ll::impl::ImageFilterModeStrings.size(), impl::ImageFilterModeStrings>(std::forward<T>(stringValue));
}

} // namespace ll

#endif // LLUVIA_CORE_IMAGE_IMAGE_FILTER_MODE_H_

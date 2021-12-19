/**
@file       ImageAxis.h
@brief      ImageAxis enum.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_IMAGE_IMAGE_AXIS_H_
#define LLUVIA_CORE_IMAGE_IMAGE_AXIS_H_

#include "lluvia/core/enums/enums.h"

// FIXME: move methods from there to lluvia/core/enums/enums.h
#include "lluvia/core/impl/enum_utils.h"

namespace ll
{

/**
@brief      Image axis enumeration.
*/
enum class ImageAxis : ll::enum_t
{
    U = 0, /**< value for U or X axis. */
    V = 1, /**< value for V or Y axis. */
    W = 2  /**< value for W or Z axis. */
};

namespace impl
{

/**
@brief Image axis strings used for converting ll::ImageAxis to std::string and vice-versa.

@sa         ll::ImageAxis enum values for this array.
*/
constexpr const std::array<std::tuple<const char *, ll::ImageAxis>, 3> ImageAxisStrings{{
    std::make_tuple("U", ll::ImageAxis::U),
    std::make_tuple("V", ll::ImageAxis::V),
    std::make_tuple("W", ll::ImageAxis::W),
}};

} // namespace impl


template <typename T = std::string>
inline T imageAxisToString(ll::ImageAxis &&axis) noexcept
{
    return impl::enumToString<ll::ImageAxis, ll::impl::ImageAxisStrings.size(), ll::impl::ImageAxisStrings>(std::forward<ll::ImageAxis>(axis));
}

template <typename T>
inline ll::ImageAxis stringToImageAxis(T &&stringValue)
{
    return impl::stringToEnum<ll::ImageAxis, T, ll::impl::ImageAxisStrings.size(), ll::impl::ImageAxisStrings>(std::forward<T>(stringValue));
}

} // namespace ll

#endif // LLUVIA_CORE_IMAGE_IMAGE_AXIS_H_

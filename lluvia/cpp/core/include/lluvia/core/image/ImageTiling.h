/**
@file       ImageUsageFlagBits.h
@brief      ImageTiling enum.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_IMAGE_IMAGE_TILING_H_
#define LLUVIA_CORE_IMAGE_IMAGE_TILING_H_

#include "lluvia/core/enums/enums.h"
#include "lluvia/core/vulkan/vulkan.hpp"


namespace ll
{

enum class ImageTiling : ll::enum_t
{
    Optimal = static_cast<ll::enum_t>(vk::ImageTiling::eOptimal),
    Linear = static_cast<ll::enum_t>(vk::ImageTiling::eLinear),
};

namespace impl
{

constexpr ll::ImageTiling fromVkImageTiling(const vk::ImageTiling& flags) noexcept
{
    return ll::ImageTiling{static_cast<ll::enum_t>(flags)};
}

constexpr vk::ImageTiling toVkImageTiling(const ll::ImageTiling& flags) noexcept
{
    // return vk::ImageTiling{static_cast<VkFlags>(flags)};
    return static_cast<vk::ImageTiling>(flags);
}

constexpr const std::array<std::tuple<const char *, ll::ImageTiling>, 2> ImageTilingStrings{{
    std::make_tuple("Optimal", ll::ImageTiling::Optimal),
    std::make_tuple("Linear", ll::ImageTiling::Linear),
}};

} // namespace impl

/**
@brief      Converts from ll::ImageTiling enum value to string

@param[in]  layout  Vulkan image layout enum.

@tparam     T          function return type. Defaults to std::string-

@return     Returns the corresponding `std::string` in ll::impl::VkImageTilingStrings for the enum value.
*/
template<typename T = std::string>
inline T ImageTilingToString(ll::ImageTiling&& layout) noexcept {
    return impl::enumToString<ll::ImageTiling, ll::impl::ImageTilingStrings.size(), ll::impl::ImageTilingStrings>(std::forward<ll::ImageTiling>(layout));
}


/**
@brief      Converts from a string-like object to ll::ImageTiling.

This function can be used either with string literals, const char* or `std::string` objects.
\p stringValue parameter is compared against the values in ll::impl::VkImageTilingStrings and the
corresponding enum value is returned. The comparison is case sensitive.

@param[in]  stringValue  string-like parameter. String literals and `std::string` objects are allowed.

@tparam     T          \p stringValue type. \p T must satisfies `std::is_convertible<T, std::string>()`

@return     ll::ImageTiling value corresponding to stringValue.

@throws std::out_of_range if \p stringValue is not found in ll::impl::VkImageTilingStrings.
*/
template<typename T>
inline ll::ImageTiling stringToImageTiling(T&& stringValue) {
    return impl::stringToEnum<ll::ImageTiling, T, ll::impl::ImageTilingStrings.size(), ll::impl::ImageTilingStrings>(std::forward<T>(stringValue));
}

} // namespace ll

#endif // LLUVIA_CORE_IMAGE_IMAGE_TILING_H_

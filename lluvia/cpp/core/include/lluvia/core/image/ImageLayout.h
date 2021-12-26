/**
@file       ImageLayout.h
@brief      ImageLayout enum.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_IMAGE_IMAGE_LAYOUT_H_
#define LLUVIA_CORE_IMAGE_IMAGE_LAYOUT_H_

#include "lluvia/core/enums/enums.h"
#include "lluvia/core/vulkan/vulkan.hpp"


namespace ll
{

enum class ImageLayout : ll::enum_t
{
    Undefined = static_cast<ll::enum_t>(vk::ImageLayout::eUndefined),
    General = static_cast<ll::enum_t>(vk::ImageLayout::eGeneral),
    ShaderReadOnlyOptimal = static_cast<ll::enum_t>(vk::ImageLayout::eShaderReadOnlyOptimal),
    TransferSrcOptimal = static_cast<ll::enum_t>(vk::ImageLayout::eTransferSrcOptimal),
    TransferDstOptimal = static_cast<ll::enum_t>(vk::ImageLayout::eTransferDstOptimal),
    Preinitialized = static_cast<ll::enum_t>(vk::ImageLayout::ePreinitialized),
};

namespace impl
{

constexpr ll::ImageLayout fromVkImageLayout(const vk::ImageLayout& flags) noexcept
{
    return ll::ImageLayout{static_cast<ll::enum_t>(flags)};
}

constexpr vk::ImageLayout toVkImageLayout(const ll::ImageLayout& flags) noexcept
{
    // return vk::ImageLayout{static_cast<VkFlags>(flags)};
    return static_cast<vk::ImageLayout>(flags);
}

constexpr const std::array<std::tuple<const char *, ll::ImageLayout>, 6> ImageLayoutStrings{{
    std::make_tuple("Undefined", ll::ImageLayout::Undefined),
    std::make_tuple("General", ll::ImageLayout::General),
    std::make_tuple("ShaderReadOnlyOptimal", ll::ImageLayout::ShaderReadOnlyOptimal),
    std::make_tuple("TransferSrcOptimal", ll::ImageLayout::TransferSrcOptimal),
    std::make_tuple("TransferDstOptimal", ll::ImageLayout::TransferDstOptimal),
    std::make_tuple("Preinitialized", ll::ImageLayout::Preinitialized)
}};

} // namespace impl

/**
@brief      Converts from vk::ImageLayout enum value to string

@param[in]  layout  Vulkan image layout enum.

@tparam     T          function return type. Defaults to std::string-

@return     Returns the corresponding `std::string` in ll::impl::VkImageLayoutStrings for the enum value.
*/
template<typename T = std::string>
inline T imageLayoutToString(ll::ImageLayout&& layout) noexcept {
    return impl::enumToString<ll::ImageLayout, ll::impl::ImageLayoutStrings.size(), ll::impl::ImageLayoutStrings>(std::forward<ll::ImageLayout>(layout));
}


/**
@brief      Converts from a string-like object to ll::ImageLayout.

This function can be used either with string literals, const char* or `std::string` objects.
\p stringValue parameter is compared against the values in ll::impl::VkImageLayoutStrings and the
corresponding enum value is returned. The comparison is case sensitive.

@param[in]  stringValue  string-like parameter. String literals and `std::string` objects are allowed.

@tparam     T          \p stringValue type. \p T must satisfies `std::is_convertible<T, std::string>()`

@return     ll::ImageLayout value corresponding to stringValue.

@throws std::out_of_range if \p stringValue is not found in ll::impl::VkImageLayoutStrings.
*/
template<typename T>
inline ll::ImageLayout stringToImageLayout(T&& stringValue) {
    return impl::stringToEnum<ll::ImageLayout, T, ll::impl::ImageLayoutStrings.size(), ll::impl::ImageLayoutStrings>(std::forward<T>(stringValue));
}

} // namespace ll

#endif // LLUVIA_CORE_IMAGE_IMAGE_LAYOUT_H_

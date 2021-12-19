/**
@file       ImageUsageFlagBits.h
@brief      Image usage flags enum.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_IMAGE_IMAGE_USAGE_FLAGS_H_
#define LLUVIA_CORE_IMAGE_IMAGE_USAGE_FLAGS_H_

#include "lluvia/core/enums/enums.h"
#include "lluvia/core/vulkan/vulkan.hpp"

// FIXME: move methods from there to lluvia/core/enums/enums.h
#include "lluvia/core/impl/enum_utils.h"

namespace ll
{

enum class ImageUsageFlagBits : ll::enum_t
{
    TransferSrc = static_cast<ll::enum_t>(vk::ImageUsageFlagBits::eTransferSrc),
    TransferDst = static_cast<ll::enum_t>(vk::ImageUsageFlagBits::eTransferDst),
    Sampled = static_cast<ll::enum_t>(vk::ImageUsageFlagBits::eSampled),
    Storage = static_cast<ll::enum_t>(vk::ImageUsageFlagBits::eStorage)
};

using ImageUsageFlags = ll::Flags<ImageUsageFlagBits, ll::enum_t>;

inline constexpr ImageUsageFlags operator|(ImageUsageFlagBits bit0, ImageUsageFlagBits bit1) noexcept
{
    return ImageUsageFlags(bit0) | bit1;
}

inline constexpr ImageUsageFlags operator&(ImageUsageFlagBits bit0, ImageUsageFlagBits bit1) noexcept
{
    return ImageUsageFlags(bit0) & bit1;
}

inline constexpr ImageUsageFlags operator^(ImageUsageFlagBits bit0, ImageUsageFlagBits bit1) noexcept
{
    return ImageUsageFlags(bit0) ^ bit1;
}

namespace impl
{

    constexpr ll::ImageUsageFlags fromVkImageUsageFlags(const vk::ImageUsageFlags &flags) noexcept
    {
        return ll::ImageUsageFlags{static_cast<ll::enum_t>(flags)};
    }

    constexpr vk::ImageUsageFlags toVkImageUsageFlags(const ll::ImageUsageFlags &flags) noexcept
    {
        return vk::ImageUsageFlags{static_cast<VkFlags>(flags)};
    }

    constexpr const std::array<std::tuple<const char *, ll::ImageUsageFlagBits>, 4> ImageUsageFlagBitsStrings{{
        std::make_tuple("TransferSrc", ll::ImageUsageFlagBits::TransferSrc),
        std::make_tuple("TransferDst", ll::ImageUsageFlagBits::TransferDst),
        std::make_tuple("Sampled", ll::ImageUsageFlagBits::Sampled),
        std::make_tuple("Storage", ll::ImageUsageFlagBits::Storage)
    }};

} // namespace impl

/**
@brief      Converts from a string vector to Vulkan ImageUsageFlags.

The comparison between string values is case sensitive.

See @VULKAN_DOC#VkImageUsageFlagBits for more information.

@param[in]  flagsVector  The flags vector. Their values must be contained
                         in impl::ImageUsageFlagBitsStrings.

@return     The reconstructed Vulkan ImageUsageFlags.
*/
inline ll::ImageUsageFlags vectorStringToImageUsageFlags(const std::vector<std::string> &flagsVector) noexcept
{
    return impl::vectorStringToFlags<ll::ImageUsageFlags, ll::ImageUsageFlagBits, ll::impl::ImageUsageFlagBitsStrings.size(), impl::ImageUsageFlagBitsStrings>(flagsVector);
}

    /**
@brief      Converst from Vulkan ImageUsageFlags to a vector of strings.

See @VULKAN_DOC#VkImageUsageFlagBits for more information.

@param[in]  flags  The Vulkan flags.

@return     A vector of string values. Each element is one of impl::ImageUsageFlagBitsStrings
*/
inline std::vector<std::string> ImageUsageFlagsToVectorString(const ll::ImageUsageFlags flags) noexcept
{
    return impl::flagsToVectorString<ll::ImageUsageFlags, ll::ImageUsageFlagBits, ll::impl::ImageUsageFlagBitsStrings.size(), ll::impl::ImageUsageFlagBitsStrings>(flags);
}

} // namespace ll

#endif // LLUVIA_CORE_IMAGE_IMAGE_USAGE_FLAGS_H_

/**
@file       MemoryPropertyFlags.h
@brief      MemoryPropertyFlags class.
@copyright  2021, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_MEMORY_MEMORY_PROPERTY_FLAGS_H_
#define LLUVIA_CORE_MEMORY_MEMORY_PROPERTY_FLAGS_H_

#include <cstdint>

#include "lluvia/core/enums/enums.h"
#include "lluvia/core/vulkan/vulkan.hpp"

namespace ll {

enum class MemoryPropertyFlagBits : ll::enum_t {
    DeviceLocal = static_cast<ll::enum_t>(vk::MemoryPropertyFlagBits::eDeviceLocal),
    HostVisible = static_cast<ll::enum_t>(vk::MemoryPropertyFlagBits::eHostVisible),
    HostCoherent = static_cast<ll::enum_t>(vk::MemoryPropertyFlagBits::eHostCoherent),
    HostCached = static_cast<ll::enum_t>(vk::MemoryPropertyFlagBits::eHostCached)
};

using MemoryPropertyFlags = ll::Flags<MemoryPropertyFlagBits, ll::enum_t>;

inline constexpr MemoryPropertyFlags operator|(MemoryPropertyFlagBits bit0, MemoryPropertyFlagBits bit1) noexcept
{
    return MemoryPropertyFlags(bit0) | bit1;
}

inline constexpr MemoryPropertyFlags operator&(MemoryPropertyFlagBits bit0, MemoryPropertyFlagBits bit1) noexcept
{
    return MemoryPropertyFlags(bit0) & bit1;
}

inline constexpr MemoryPropertyFlags operator^(MemoryPropertyFlagBits bit0, MemoryPropertyFlagBits bit1) noexcept
{
    return MemoryPropertyFlags(bit0) ^ bit1;
}

// inline constexpr MemoryPropertyFlags operator~(MemoryPropertyFlagBits bits) noexcept
// {
//     return ~(MemoryPropertyFlags(bits));
// }

namespace impl {

    constexpr ll::MemoryPropertyFlags fromVkMemoryPropertyFlags(const vk::MemoryPropertyFlags& flags) noexcept
    {
        return ll::MemoryPropertyFlags { static_cast<ll::enum_t>(flags) };
    }

    /**
    String values for Vulkan VkMemoryPropertyFlagBits values.

    See @VULKAN_DOC#VkMemoryPropertyFlagBits for more information.
    */
    constexpr const std::array<std::tuple<const char*, ll::MemoryPropertyFlagBits>, 4> MemoryPropertyFlagBitsStrings { { std::make_tuple("DeviceLocal", ll::MemoryPropertyFlagBits::DeviceLocal),
        std::make_tuple("HostCached", ll::MemoryPropertyFlagBits::HostCached),
        std::make_tuple("HostCoherent", ll::MemoryPropertyFlagBits::HostCoherent),
        std::make_tuple("HostVisible", ll::MemoryPropertyFlagBits::HostVisible) } };

} // namespace impl

/**
@brief      Converts from a string vector to Vulkan MemoryPropertyFlags.

The comparison between string values is case sensitive.

See @VULKAN_DOC#VkMemoryPropertyFlagBits for more information.

@param[in]  flagsVector  The flags vector. Their values must be contained
                         in impl::VkMemoryPropertyFlagBitsStrings.

@return     The reconstructed Vulkan MemoryPropertyFlags.
*/
inline ll::MemoryPropertyFlags vectorStringToMemoryPropertyFlags(const std::vector<std::string>& flagsVector) noexcept
{
    return impl::vectorStringToFlags<ll::MemoryPropertyFlags, ll::MemoryPropertyFlagBits, ll::impl::MemoryPropertyFlagBitsStrings.size(), impl::MemoryPropertyFlagBitsStrings>(flagsVector);
}

/**
@brief      Converts from Vulkan MemoryPropertyFlags to a vector of strings.

See @VULKAN_DOC#VkMemoryPropertyFlagBits for more information.

@param[in]  flags  The Vulkan flags.

@return     A vector of string values. Each element is one of impl::VkMemoryPropertyFlagBitsStrings
*/
inline std::vector<std::string> memoryPropertyFlagsToVectorString(const ll::MemoryPropertyFlags flags) noexcept
{
    return impl::flagsToVectorString<ll::MemoryPropertyFlags, ll::MemoryPropertyFlagBits, ll::impl::MemoryPropertyFlagBitsStrings.size(), ll::impl::MemoryPropertyFlagBitsStrings>(flags);
}

} // namespace ll

#endif // LLUVIA_CORE_MEMORY_MEMORY_PROPERTY_FLAGS_H_

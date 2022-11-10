/**
@file       BufferUsageFlagBits.h
@brief      Buffer usage flags enum.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_BUFFER_BUFFER_USAGE_FLAGS_H_
#define LLUVIA_CORE_BUFFER_BUFFER_USAGE_FLAGS_H_

#include "lluvia/core/enums/enums.h"
#include "lluvia/core/vulkan/vulkan.hpp"

namespace ll {

enum class BufferUsageFlagBits : ll::enum_t {
    StorageBuffer = static_cast<ll::enum_t>(vk::BufferUsageFlagBits::eStorageBuffer),
    TransferDst   = static_cast<ll::enum_t>(vk::BufferUsageFlagBits::eTransferDst),
    TransferSrc   = static_cast<ll::enum_t>(vk::BufferUsageFlagBits::eTransferSrc),
    UniformBuffer = static_cast<ll::enum_t>(vk::BufferUsageFlagBits::eUniformBuffer)
};

using BufferUsageFlags = ll::Flags<BufferUsageFlagBits, ll::enum_t>;

inline constexpr BufferUsageFlags operator|(BufferUsageFlagBits bit0, BufferUsageFlagBits bit1) noexcept
{
    return BufferUsageFlags(bit0) | bit1;
}

inline constexpr BufferUsageFlags operator&(BufferUsageFlagBits bit0, BufferUsageFlagBits bit1) noexcept
{
    return BufferUsageFlags(bit0) & bit1;
}

inline constexpr BufferUsageFlags operator^(BufferUsageFlagBits bit0, BufferUsageFlagBits bit1) noexcept
{
    return BufferUsageFlags(bit0) ^ bit1;
}

namespace impl {

    constexpr ll::BufferUsageFlags fromVkBufferUsageFlags(const vk::BufferUsageFlags& flags) noexcept
    {
        return ll::BufferUsageFlags {static_cast<ll::enum_t>(flags)};
    }

    inline vk::BufferUsageFlags toVkBufferUsageFlags(const ll::BufferUsageFlags& flags) noexcept
    {
        return vk::BufferUsageFlags {static_cast<VkFlags>(flags)};
    }

    constexpr const std::array<std::tuple<const char*, ll::BufferUsageFlagBits>, 4> BufferUsageFlagBitsStrings {{std::make_tuple("StorageBuffer", ll::BufferUsageFlagBits::StorageBuffer),
        std::make_tuple("TransferDst", ll::BufferUsageFlagBits::TransferDst),
        std::make_tuple("TransferSrc", ll::BufferUsageFlagBits::TransferSrc),
        std::make_tuple("UniformBuffer", ll::BufferUsageFlagBits::UniformBuffer)}};

} // namespace impl

/**
@brief      Converts from a string vector to Vulkan BufferUsageFlags.

The comparison between string values is case sensitive.

See @VULKAN_DOC#VkBufferUsageFlagBits for more information.

@param[in]  flagsVector  The flags vector. Their values must be contained
                         in impl::VkBufferUsageFlagBitsStrings.

@return     The reconstructed Vulkan BufferUsageFlags.
*/
inline ll::BufferUsageFlags vectorStringToBufferUsageFlags(const std::vector<std::string>& flagsVector) noexcept
{
    return impl::vectorStringToFlags<ll::BufferUsageFlags, ll::BufferUsageFlagBits, ll::impl::BufferUsageFlagBitsStrings.size(), impl::BufferUsageFlagBitsStrings>(flagsVector);
}

/**
@brief      Converst from Vulkan BufferUsageFlags to a vector of strings.

See @VULKAN_DOC#VkBufferUsageFlagBits for more information.

@param[in]  flags  The Vulkan flags.

@return     A vector of string values. Each element is one of impl::VkBufferUsageFlagBitsStrings
*/
inline std::vector<std::string> bufferUsageFlagsToVectorString(const ll::BufferUsageFlags flags) noexcept
{
    return impl::flagsToVectorString<ll::BufferUsageFlags, ll::BufferUsageFlagBits, ll::impl::BufferUsageFlagBitsStrings.size(), ll::impl::BufferUsageFlagBitsStrings>(flags);
}

} // namespace ll

#endif // LLUVIA_CORE_BUFFER_BUFFER_USAGE_FLAGS_H_

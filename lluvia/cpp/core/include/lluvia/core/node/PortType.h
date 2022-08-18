/**
@file       PortType.h
@brief      PortType enum.
@copyright  2022, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_NODE_PORT_TYPE_H_
#define LLUVIA_CORE_NODE_PORT_TYPE_H_

#include "lluvia/core/enums/enums.h"
#include "lluvia/core/vulkan/vulkan.hpp"


namespace ll
{

/**
@brief      Port types.

@sa ll::impl::ParameterTypeStrings string values for this enum.
*/
enum class PortType : ll::enum_t {
    Buffer           = 0,   /**< value for ll::Buffer type. */
    ImageView        = 1,   /**< value for ll::ImageView without pixel sampler.*/
    SampledImageView = 2,   /**< value for ll::ImageView objects coupled with a pixel sampler. */
    UniformBuffer    = 3,   /**< value for ll::Buffer objects allocated to be used as uniform buffer. */
};

namespace impl
{

    /**
    @brief Port type string values used for converting ll::PortType to std::string and vice-versa.

    @sa ll::PortType enum values for this array.
    */
    constexpr const std::array<std::tuple<const char *, ll::PortType>, 4> PortTypeStrings{{
        std::make_tuple("Buffer", ll::PortType::Buffer),
        std::make_tuple("ImageView", ll::PortType::ImageView),
        std::make_tuple("SampledImageView", ll::PortType::SampledImageView),
        std::make_tuple("UniformBuffer", ll::PortType::UniformBuffer),
    }};

} // namespace impl

/**
@brief      Converts from ll::PortType enum value to string.

@param[in]  value

@tparam     T          function return type. Defaults to std::string.

@return     Returns the corresponding std::string in ll::impl::PortTypeStrings for the enum value.
*/
template <typename T = std::string>
inline T portTypeToString(ll::PortType &&value) noexcept
{
    return ll::impl::enumToString<ll::PortType, ll::impl::PortTypeStrings.size(), impl::PortTypeStrings>(std::forward<ll::PortType>(value));
}

template <typename T = std::string>
inline T portTypeToString(const ll::PortType& value) noexcept {
    return ll::impl::enumToString<ll::PortType, ll::impl::PortTypeStrings.size(), impl::PortTypeStrings>(value);
}

/**
@brief      Converts from ll::PortType enum to Vulkan DescriptorType.

@param[in]  param  The parameter.

@return     The corresponding Vulkan descriptor type for \p param.
*/
vk::DescriptorType portTypeToVkDescriptorType(const ll::PortType &param);

/**
@brief      Converts from Vulkan DescriptorType to ll::PortType enum.

@param[in]  vkDescType  The Vulkan description type.

@return     The corresponding ll::PortType.

@throws     std::system_error if there is no associated ll::PortType value for \p vkDescType.
                              Using the values returned by ll::portTypeToVkDescriptorType is guaranteed
                              to not throw exception.
*/
ll::PortType vkDescriptorTypeToPortType(const vk::DescriptorType &vkDescType);

/**
@brief      Converts from a string-like object to ll::PortType enum.

@param[in]  stringValue  string-like parameter. String literals and `std::string` objects are allowed.

@tparam     T            \p stringValue type. \p T must satisfies `std::is_convertible<T, std::string>()`

@return     ll::PortType value corresponding to stringValue

@throws std::out_of_range if \p stringValue is not found in ll::impl::ObjectTypeStrings.
*/
template <typename T>
inline ll::PortType stringToPortType(T &&stringValue)
{
    return impl::stringToEnum<ll::PortType, T, ll::impl::PortTypeStrings.size(), impl::PortTypeStrings>(std::forward<T>(stringValue));
}

} // namespace ll

#endif // LLUVIA_CORE_NODE_PORT_TYPE_H_

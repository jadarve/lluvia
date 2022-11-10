/**
@file       DeviceType.h
@brief      DeviceType enum.
@copyright  2022, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_DEVICE_DEVICE_TYPE_H_
#define LLUVIA_CORE_DEVICE_DEVICE_TYPE_H_

#include "lluvia/core/enums/enums.h"
#include "lluvia/core/vulkan/vulkan.hpp"

namespace ll {

/**
@brief      Image axis enumeration.
*/
enum class DeviceType : ll::enum_t {
    Other = static_cast<ll::enum_t>(vk::PhysicalDeviceType::eOther),
    DiscreteGPU = static_cast<ll::enum_t>(vk::PhysicalDeviceType::eDiscreteGpu),
    IntegratedGPU = static_cast<ll::enum_t>(vk::PhysicalDeviceType::eIntegratedGpu),
    VirtualGPU = static_cast<ll::enum_t>(vk::PhysicalDeviceType::eVirtualGpu),
    CPU = static_cast<ll::enum_t>(vk::PhysicalDeviceType::eCpu),
};

namespace impl {

    constexpr ll::DeviceType fromVkPhysicalDeviceType(const vk::PhysicalDeviceType& physicalDeviceType) noexcept
    {
        return ll::DeviceType { static_cast<ll::enum_t>(physicalDeviceType) };
    }

    /**
    @brief Image axis strings used for converting ll::DeviceType to std::string and vice-versa.

    @sa         ll::DeviceType enum values for this array.
    */
    constexpr const std::array<std::tuple<const char*, ll::DeviceType>, 5> DeviceTypeStrings { {
        std::make_tuple("Other", ll::DeviceType::Other),
        std::make_tuple("DiscreteGPU", ll::DeviceType::DiscreteGPU),
        std::make_tuple("IntegratedGPU", ll::DeviceType::IntegratedGPU),
        std::make_tuple("VirtualGPU", ll::DeviceType::VirtualGPU),
        std::make_tuple("CPU", ll::DeviceType::CPU),
    } };

} // namespace impl

template <typename T = std::string>
inline T deviceTypeToString(ll::DeviceType&& deviceType) noexcept
{
    return impl::enumToString<ll::DeviceType, ll::impl::DeviceTypeStrings.size(), ll::impl::DeviceTypeStrings>(std::forward<ll::DeviceType>(deviceType));
}

template <typename T>
inline ll::DeviceType stringToDeviceType(T&& stringValue)
{
    return impl::stringToEnum<ll::DeviceType, T, ll::impl::DeviceTypeStrings.size(), ll::impl::DeviceTypeStrings>(std::forward<T>(stringValue));
}

} // namespace ll

#endif // LLUVIA_CORE_DEVICE_DEVICE_TYPE_H_

/**
@file       SessionDescriptor.h
@brief      SessionDescriptor class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/
#ifndef LLUVIA_CORE_SESSION_DESCRIPTOR_H_
#define LLUVIA_CORE_SESSION_DESCRIPTOR_H_

#include <optional>

#include "lluvia/core/device/DeviceDescriptor.h"

namespace ll {

class SessionDescriptor {

public:
    /**
    @brief     Enable validation layers and debug callbacks for this session.

    When enabled, the vulkan VK_LAYER_KHRONOS_validation
    layer is activated, and messages about bad usage of the API
    will appear.

    @param[in] enable whether or not debug is enabled.

    @return    A reference to this object.
     */
    SessionDescriptor& enableDebug(const bool enable) noexcept;

    /**

    @return whether or not debugging is enabled.
     */
    bool isDebugEnabled() const noexcept;

    /**
    @brief   Sets the device descriptor used to create a session

    @return    A reference to this object.
     */
    SessionDescriptor &setDeviceDescriptor(const ll::DeviceDescriptor &deviceDescriptor) noexcept;

    std::optional<ll::DeviceDescriptor> getDeviceDescriptor() const noexcept;

private:
    bool m_enableDebug {false};

    std::optional<ll::DeviceDescriptor> m_deviceDescriptor {};
};

} // namespace ll

#endif //LLUVIA_CORE_SESSION_DESCRIPTOR_H_

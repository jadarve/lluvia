/**
@file       SessionDescriptor.h
@brief      SessionDescriptor class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/
#ifndef LLUVIA_CORE_SESSIONDESCRIPTOR_H_
#define LLUVIA_CORE_SESSIONDESCRIPTOR_H_

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

private:
    bool m_enableDebug {false};
};

} // namespace ll

#endif //LLUVIA_CORE_SESSIONDESCRIPTOR_H_

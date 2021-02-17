/**
@file       SessionDescriptor.cpp
@brief      SessionDescriptor class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#include "lluvia/core/SessionDescriptor.h"

namespace ll {

SessionDescriptor& SessionDescriptor::enableDebug(const bool enable) noexcept {
    m_enableDebug = enable;
    return *this;
}

bool SessionDescriptor::isDebugEnabled() const noexcept {
    return m_enableDebug;
}

} // namespace ll

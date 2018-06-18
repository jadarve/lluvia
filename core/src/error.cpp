/**
@file       error.cpp
@brief      error related classes and methods.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#include "lluvia/core/error.h"

namespace ll {

ErrorCategory& ErrorCategory::getInstance() noexcept {
    static ErrorCategory instance {};
    return instance;
}

} // namespace ll
/**
@file       error.h
@brief      error related classes and methods.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_ERROR_H_
#define LLUVIA_CORE_ERROR_H_

#include "lluvia/core/impl/enum_utils.h"

#include <system_error>

namespace ll {


/**
@brief      Error codes
*/
enum class ErrorCode : int32_t {
    MemoryMapFailed       = 1,      /**< Memory mapping operation (map or unmap) failed */
    ObjectAllocationError = 0,      /**< Error trying to allocate objects in a memory */
};


namespace impl {

    /**
    String values for ll::ErrorCode enum.
    */
    constexpr const std::array<const char*, 2> ErrorCodeStrings{{
        "MemoryMapFailed",
        "ObjectAllocationError"
    }};

} // namespace impl


/**
@brief      Converts from ll::ErrorCode enum value to std::string.

@param[in]  value      

@tparam     T          function return type. Defaults to std::string.

@return     Returns the corresponding `std::string` in ll::impl::ErrorCodeStrings for the enum value.
*/
template<typename T = std::string>
inline T errorCodeToString(ll::ErrorCode&& value) {
    return impl::enumToString<ll::ErrorCode, ll::impl::ErrorCodeStrings.size(), ll::impl::ErrorCodeStrings>(std::forward<ll::ErrorCode>(value));
}


class ErrorCategory : public std::error_category {

public:
    static ErrorCategory& getInstance() noexcept;

    virtual const char* name() const noexcept override { return "ll::ErrorCode"; }
    virtual std::string message(int ev) const override { return errorCodeToString(static_cast<ll::ErrorCode>(ev)); }
};


inline std::error_code createErrorCode(ll::ErrorCode errorCode) {
    return std::error_code {static_cast<int>(errorCode), ErrorCategory::getInstance()};
}

} // namespace ll

#endif // LLUVIA_CORE_ERROR_H_

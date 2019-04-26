/**
@file       error.h
@brief      error related classes and methods.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_ERROR_H_
#define LLUVIA_CORE_ERROR_H_

#include "lluvia/core/impl/enum_utils.h"

#include <array>
#include <string>
#include <system_error>
#include <tuple>


namespace ll {


/**
@brief      Error codes
*/
enum class ErrorCode : int32_t {
    EnumConversionFailed      = 0,      /**< Conversion between enum types failed */
    MemoryMapFailed           = 1,      /**< Memory mapping operation (map or unmap) failed */
    ObjectAllocationError     = 2,      /**< Error trying to allocate objects in a memory */
    ParameterBindingError     = 3,      /**< Error binding a parameter in a ll::ComputeNode */
    InvalidShaderFunctionName = 5,      /**< Shader function name has invalid name, such as empty string */
    InvalidShaderProgram      = 6,      /**< Shader program is not valid*/
    BufferCopyError           = 7,      /**< Error copying data between buffers*/
    ProgramCompilationError   = 8,      /**< Error compiling shader module for program*/
    InvalidLocalShape         = 9,      /**< Local shape passed to a Compute node is invalid*/
    InvalidGridShape          = 10,     /**< Grid shape for a ComputeNode is invalid */
    BadEnumCasting            = 11      /**< Bad casting of integral value to enum type */
};


namespace impl {

    /**
    String values for ll::ErrorCode enum.
    */
    constexpr const std::array<std::tuple<const char*, ll::ErrorCode>, 11> ErrorCodeStrings {{
        std::make_tuple("EnumConversionFailed"      , ll::ErrorCode::EnumConversionFailed),
        std::make_tuple("MemoryMapFailed"           , ll::ErrorCode::MemoryMapFailed),
        std::make_tuple("ObjectAllocationError"     , ll::ErrorCode::ObjectAllocationError),
        std::make_tuple("ParameterBindingError"     , ll::ErrorCode::ParameterBindingError),
        std::make_tuple("InvalidShaderFunctionName" , ll::ErrorCode::InvalidShaderFunctionName),
        std::make_tuple("InvalidShaderProgram"      , ll::ErrorCode::InvalidShaderProgram),
        std::make_tuple("BufferCopyError"           , ll::ErrorCode::BufferCopyError),
        std::make_tuple("ProgramCompilationError"   , ll::ErrorCode::ProgramCompilationError),
        std::make_tuple("InvalidLocalShape"         , ll::ErrorCode::InvalidLocalShape),
        std::make_tuple("InvalidGridShape"          , ll::ErrorCode::InvalidGridShape),
        std::make_tuple("BadEnumCasting"            , ll::ErrorCode::BadEnumCasting),
    }};

} // namespace impl


/**
@brief      Converts from ll::ErrorCode enum value to std::string.

@param[in]  value      

@tparam     T          function return type. Defaults to std::string.

@return     Returns the corresponding `std::string` in ll::impl::ErrorCodeStrings for the enum value.
*/
template<typename T = std::string>
inline T errorCodeToString(ll::ErrorCode&& value) noexcept {
    return impl::enumToString<ll::ErrorCode, ll::impl::ErrorCodeStrings.size(), ll::impl::ErrorCodeStrings>(std::forward<ll::ErrorCode>(value));
}


/**
@brief      Class for error code category.
*/
class ErrorCategory : public std::error_category {

public:
    /**
    @brief      Gets a singleton instance.
    
    @return     The instance.
    */
    static ErrorCategory& getInstance() noexcept;


    /**
    @brief      Gets the error category name.
    
    @return     The name.
    */
    virtual const char* name() const noexcept override { return "ll::ErrorCode"; }


    /**
    @brief      Converts from an error code to a string message.
    
    @param[in]  ev    error code. The value must correspond with one of ll::ErrorCode values.
    
    @return     The corresponding string for \p ev.
    */
    virtual std::string message(int ev) const override { return errorCodeToString(static_cast<ll::ErrorCode>(ev)); }
};


/**
@brief      Creates an error code.

@param[in]  errorCode  The error code

@return     a std::error_code instance.
*/
inline std::error_code createErrorCode(ll::ErrorCode errorCode) {
    return std::error_code {static_cast<int>(errorCode), ErrorCategory::getInstance()};
}


/**
@brief      Throws a std::system_error exception if the condition passed is true.

@param[in]  condition  The condition.
@param[in]  errorCode  The error code.
@param      msg        The error message.

@tparam     T          Type of the error message. It must be convertible to std::string.
*/
template<typename T>
void throwSystemErrorIf(bool condition, ll::ErrorCode errorCode, T&& msg) {

    static_assert(std::is_convertible<T, std::string>(), "T must be a string-like type");    

    if (condition) {
        throw std::system_error(createErrorCode(errorCode), msg);
    }
}

} // namespace ll

#endif // LLUVIA_CORE_ERROR_H_

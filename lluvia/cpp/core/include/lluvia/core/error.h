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
    EnumConversionFailed,      /**< Conversion between enum types failed */
    MemoryMapFailed,           /**< Memory mapping operation (map or unmap) failed */
    ObjectAllocationError,     /**< Error trying to allocate objects in a memory */
    PortBindingError,          /**< Error binding a port to a ll::Node */
    InvalidShaderFunctionName, /**< Shader function name has invalid name, such as empty string */
    InvalidShaderProgram,      /**< Shader program is not valid*/
    BufferCopyError,           /**< Error copying data between buffers*/
    ProgramCompilationError,   /**< Error compiling shader module for program*/
    InvalidLocalShape,         /**< Local shape passed to a Compute node is invalid*/
    InvalidGridShape,          /**< Grid shape for a ComputeNode is invalid */
    BadEnumCasting,            /**< Bad casting of integral value to enum type */
    PhysicalDevicesNotFound,   /**< No physical devices found */
    KeyNotFound,               /**< Key not found in a given look up method */
    MemoryCreationError,       /**< Error creating ll::Memory object */
    InvalidNodeState,          /**< Invalid ll::Node state */
    InterpreterError,          /**< Error evaluating interpreter script*/
    PushConstantError,         /**< Error regarding push constant operation*/
    IOError,                   /**< IO Error accessing files*/
    InvalidArgument,           /**< Invalid argument*/
    InstanceCreationError,     /**< Incompatible driver */
    SessionLost,               /**< An object is trying to access a Session already destroyed */
    LayerNotFound,             /**< A layer required to create a Session was not found */
    ExtensionNotFound,         /**< An extension required to create a Session was not found */
    PipelineCreationError,     /**< Error creating a vulkan pipeline object */
    VulkanError                /**< Error calling a vulkan function that does not fit on any of the categories above */
};

namespace impl {

    /**
    String values for ll::ErrorCode enum.
    */
constexpr const std::array<std::tuple<const char *, ll::ErrorCode>, 25> ErrorCodeStrings{{
    std::make_tuple("EnumConversionFailed", ll::ErrorCode::EnumConversionFailed),
    std::make_tuple("MemoryMapFailed", ll::ErrorCode::MemoryMapFailed),
    std::make_tuple("ObjectAllocationError", ll::ErrorCode::ObjectAllocationError),
    std::make_tuple("PortBindingError", ll::ErrorCode::PortBindingError),
    std::make_tuple("InvalidShaderFunctionName", ll::ErrorCode::InvalidShaderFunctionName),
    std::make_tuple("InvalidShaderProgram", ll::ErrorCode::InvalidShaderProgram),
    std::make_tuple("BufferCopyError", ll::ErrorCode::BufferCopyError),
    std::make_tuple("ProgramCompilationError", ll::ErrorCode::ProgramCompilationError),
    std::make_tuple("InvalidLocalShape", ll::ErrorCode::InvalidLocalShape),
    std::make_tuple("InvalidGridShape", ll::ErrorCode::InvalidGridShape),
    std::make_tuple("BadEnumCasting", ll::ErrorCode::BadEnumCasting),
    std::make_tuple("PhysicalDevicesNotFound", ll::ErrorCode::PhysicalDevicesNotFound),
    std::make_tuple("KeyNotFound", ll::ErrorCode::KeyNotFound),
    std::make_tuple("MemoryCreationError", ll::ErrorCode::MemoryCreationError),
    std::make_tuple("InvalidNodeState", ll::ErrorCode::InvalidNodeState),
    std::make_tuple("InterpreterError", ll::ErrorCode::InterpreterError),
    std::make_tuple("PushConstantError", ll::ErrorCode::PushConstantError),
    std::make_tuple("IOError", ll::ErrorCode::IOError),
    std::make_tuple("InvalidArgument", ll::ErrorCode::InvalidArgument),
    std::make_tuple("InstanceCreationError", ll::ErrorCode::InstanceCreationError),
    std::make_tuple("SessionLost", ll::ErrorCode::SessionLost),
    std::make_tuple("LayerNotFound", ll::ErrorCode::LayerNotFound),
    std::make_tuple("ExtensionNotFound", ll::ErrorCode::ExtensionNotFound),
    std::make_tuple("PipelineCreationError", ll::ErrorCode::PipelineCreationError),
    std::make_tuple("VulkanError", ll::ErrorCode::VulkanError)
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

/**
@brief      Throws a std::system_error exception with error code and message.

@param[in]  condition  The condition.
@param[in]  errorCode  The error code.
@param      msg        The error message.

@tparam     T          Type of the error message. It must be convertible to std::string.
*/
template <typename T>
void throwSystemError(ll::ErrorCode errorCode, T &&msg) {

    static_assert(std::is_convertible<T, std::string>(), "T must be a string-like type");
    throw std::system_error(createErrorCode(errorCode), msg);
}

} // namespace ll

#endif // LLUVIA_CORE_ERROR_H_

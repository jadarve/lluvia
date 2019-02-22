/**
@file       utils.h
@brief      utility functions.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_UTILS_H_
#define LLUVIA_CORE_UTILS_H_

#include "lluvia/core/types.h"

#include <vulkan/vulkan.hpp>

#include <array>
#include <cstdint>
#include <string>
#include <vector>
#include <type_traits>


namespace ll {


/**
@brief      Converts a binary array to base-64 string.

@param[in]  ptr   The pointer to the binary data.
@param[in]  size  The size of the data in bytes.

@return     the encoded base-64 string.

@sa         ll::fromBase64 Converts a std::string with base-64 characters to binary.
*/
std::string toBase64(const void* ptr, const size_t size);


/**
@brief      Converts a std::string with base-64 characters to binary.

@param[in]  code  The base-64 code.

@return     The decoded base-64 binary array.

@throws     std::runtime_error if any character in \p code does not belong to the base-64 alphabet.

@sa         ll::toBase64 Converts a binary array to base-64 string.
*/
std::vector<uint8_t> fromBase64(const std::string& code);


/**
@brief      Calculates the grid shape provided local and global shapes.

The calculation is done as:

@code
    grid.x = ceil(global.x / local.x)
    grid.y = ceil(global.y / local.y)
    grid.z = ceil(global.z / local.z)
@endcode

This method does not control the case of dividing by zero.

@param[in]  localShape   The local shape. Its values must be greater than zero.
@param[in]  globalShape  The global shape.

@tparam     F            Floating point type used for the calculations. Defaults to float.

@return     The grid shape.
*/
template<typename F=float>
ll::vec3ui configureGridShape(const vec3ui& localShape, const vec3ui& globalShape) noexcept {

    static_assert(std::is_floating_point<F>::value, "F must be a floating point type");

    return vec3ui {
        static_cast<uint32_t>(std::ceil(static_cast<F>(globalShape.x) / static_cast<F>(localShape.x))),
        static_cast<uint32_t>(std::ceil(static_cast<F>(globalShape.y) / static_cast<F>(localShape.y))),
        static_cast<uint32_t>(std::ceil(static_cast<F>(globalShape.z) / static_cast<F>(localShape.z)))
    };
}

} // namespace ll

#endif // LLUVIA_CORE_UTILS_H_

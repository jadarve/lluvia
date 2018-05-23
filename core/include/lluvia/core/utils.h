/**
@file       utils.h
@brief      utility functions.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_UTILS_H_
#define LLUVIA_CORE_UTILS_H_

#include <vulkan/vulkan.hpp>

#include <array>
#include <cstdint>
#include <string>
#include <vector>


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

} // namespace ll

#endif // LLUVIA_CORE_UTILS_H_

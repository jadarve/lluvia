/**
@file       utils.h
@brief      utility functions.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_UTILS_H_
#define LLUVIA_CORE_UTILS_H_

#include "lluvia/core/image/ImageLayout.h"
#include "lluvia/core/types.h"
#include "lluvia/core/vulkan/vulkan.hpp"

#include <array>
#include <cmath>
#include <cstdint>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

namespace ll {

class Image;
class ImageDescriptor;
class ImageView;
class ImageViewDescriptor;
class Memory;
class Session;

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
template <typename F = float>
ll::vec3ui configureGridShape(const vec3ui& localShape, const vec3ui& globalShape) noexcept
{

    static_assert(std::is_floating_point<F>::value, "F must be a floating point type");

    return vec3ui {
        static_cast<uint32_t>(std::ceil(static_cast<F>(globalShape.x) / static_cast<F>(localShape.x))),
        static_cast<uint32_t>(std::ceil(static_cast<F>(globalShape.y) / static_cast<F>(localShape.y))),
        static_cast<uint32_t>(std::ceil(static_cast<F>(globalShape.z) / static_cast<F>(localShape.z)))};
}

/**
@brief      Creates and initialize a ll::Image object.

@param      session        The session used for initiliazing the image.
@param      memory         The memory the image will be create from.
@param[in]  desc           The image descriptor.
@param[in]  initialLayout  The initial layout. Defaults to vk::ImageLayout::eGeneral

@return     A new ll::Image object.
*/
std::shared_ptr<ll::Image> createAndInitImage(
    std::shared_ptr<ll::Session>& session,
    std::shared_ptr<ll::Memory>&  memory,
    const ll::ImageDescriptor&    desc,
    const ll::ImageLayout         initialLayout = ll::ImageLayout::General);

/**
@brief      Creates and initialize a ll::ImageView object.

@param      session         The session used for initiliazing the underlying ll::Image object.
@param      memory          The memory used for allocating the image.
@param[in]  imgDescriptor   The image descriptor.
@param[in]  viewDescriptor  The view descriptor.
@param[in]  initialLayout   The initial layout. Defaults to ll::ImageLayout::eGeneral.

@return     A new ll::ImageViewObject.
*/
std::shared_ptr<ll::ImageView> createAndInitImageView(
    std::shared_ptr<ll::Session>&  session,
    std::shared_ptr<ll::Memory>&   memory,
    const ll::ImageDescriptor&     imgDescriptor,
    const ll::ImageViewDescriptor& viewDescriptor,
    const ll::ImageLayout          initialLayout = ll::ImageLayout::General);

} // namespace ll

#endif // LLUVIA_CORE_UTILS_H_

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

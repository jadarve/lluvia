/**
@file       Image.h
@brief      Image class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_IMAGE_H_
#define LLUVIA_CORE_IMAGE_H_

#include "lluvia/core/ImageDescriptor.h"
#include "lluvia/core/MemoryAllocationInfo.h"
#include "lluvia/core/Object.h"
#include "lluvia/core/impl/enum_utils.h"

#include <vulkan/vulkan.hpp>

#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <tuple>
#include <vector>


namespace ll {

class CommandBuffer;
class ComputeNode;
class ComputeGraph;
class ImageView;
class ImageViewDescriptor;
class Memory;
class Session;


namespace impl {

    /**
    String values for Vulkan VkImageUsageFlagBits values.

    See @VULKAN_DOC#VkImageUsageFlagBits for more information.
    */
    constexpr const std::array<std::tuple<const char*, vk::ImageUsageFlagBits>, 8> VkImageUsageFlagBitsStrings {{
        {"IMAGE_USAGE_TRANSFER_SRC"             , vk::ImageUsageFlagBits::eTransferSrc},
        {"IMAGE_USAGE_TRANSFER_DST"             , vk::ImageUsageFlagBits::eTransferDst},
        {"IMAGE_USAGE_SAMPLED"                  , vk::ImageUsageFlagBits::eSampled},
        {"IMAGE_USAGE_STORAGE"                  , vk::ImageUsageFlagBits::eStorage},
        {"IMAGE_USAGE_COLOR_ATTACHMENT"         , vk::ImageUsageFlagBits::eColorAttachment},
        {"IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT" , vk::ImageUsageFlagBits::eDepthStencilAttachment},
        {"IMAGE_USAGE_TRANSIENT_ATTACHMENT"     , vk::ImageUsageFlagBits::eTransientAttachment},
        {"IMAGE_USAGE_INPUT_ATTACHMENT"         , vk::ImageUsageFlagBits::eInputAttachment},
    }};

} // namespace impl


/**
@brief      Converts from a string vector to Vulkan ImageUsageFlags.

The comparison between string values is case sensitive.

See @VULKAN_DOC#VkImageUsageFlagBits for more information.

@param[in]  flagsVector  The flags vector. Their values must be contained
                         in impl::VkImageUsageFlagBitsStrings.

@return     The reconstructed Vulkan ImageUsageFlags.
*/
inline vk::ImageUsageFlags vectorStringToImageUsageFlags(const std::vector<std::string>& flagsVector) noexcept {
    return impl::vectorStringToFlags<vk::ImageUsageFlags, vk::ImageUsageFlagBits, impl::VkImageUsageFlagBitsStrings.size(), impl::VkImageUsageFlagBitsStrings>(flagsVector);
}


/**
@brief      Converst from Vulkan ImageUsageFlags to a vector of strings.

See @VULKAN_DOC#VkImageUsageFlagBits for more information.

@param[in]  flags  The Vulkan flags.

@return     A vector of string values. Each element is one of impl::VkImageUsageFlagBitsStrings
*/
inline std::vector<std::string> ImageUsageFlagsToVectorString(const vk::ImageUsageFlags flags) noexcept {
    return impl::flagsToVectorString<vk::ImageUsageFlags, vk::ImageUsageFlagBits, impl::VkImageUsageFlagBitsStrings.size(), impl::VkImageUsageFlagBitsStrings>(flags);
}


/**
@brief      Objects to manage images.

Image objects are created by calling ll::Memory::createImage using
a valid ll::ImageDescriptor.

@code
    auto session = ll::Session::create();

    // device-local memory with page size of 2048 bytes
    auto memory = session->createMemory(vk::MemoryPropertyFlagBits::eDeviceLocal, 2048);

    // 8-bit unsigned, 4-channel, 640x480 image.
    auto desc = ll::ImageDescriptor{}
                    .setWidth(640)
                    .setHeight(480)
                    .setChannelType(ll::ChannelType::Uint8)
                    .setChannelCount(4);

    auto image = memory->createImage(desc);
@endcode

Images can be passed directly to compute nodes or through ll::ImageView
objects. In the first case, the object is mapped to a GLSL image type while
in the second case, it is mapped to sampler type. 

\b TODO
- Example of compute node using image objects.
- Explain image layout transitions.

*/
class Image: public Object,
             public std::enable_shared_from_this<ll::Image> {

public:
    Image()                = delete;
    Image(const Image&)    = delete;
    Image(Image&&)         = delete;

    ~Image();

    Image& operator = (const Image&) = delete;
    Image& operator = (Image&&)      = delete;

    ll::ObjectType getType() const noexcept override;


    /**
    @brief      Gets the allocation information.
    
    @return     The allocation information.
    */
    ll::MemoryAllocationInfo getAllocationInfo() const noexcept;


    /**
    @brief      Gets the memory allocation size in bytes.

    This methods is equivalent to calling `getAllocationInfo().size`.
    
    @return     The image size in bytes.
    */
    uint64_t getSize() const noexcept;


    /**
    @brief      Gets the Vulkan image usage flags.
    
    See @VULKAN_DOC#VkImageUsageFlagBits
    for more information.

    @return     The usage flags.
    */
    vk::ImageUsageFlags getUsageFlags() const noexcept;


    /**
    @brief      Gets the Vulkan image layout.

    The layout is changed through invocations of Session::changeImageLayout
    or CommandBuffer::changeImageLayout.

    See @VULKAN_DOC#_vkimagelayout_3
    
    @return     The image layout.
    */
    vk::ImageLayout     getLayout()     const noexcept;


    /**
    @brief      Gets the channel type.
    
    @return     The channel type.
    */
    ll::ChannelType getChannelType() const noexcept;


    /**
    @brief      Gets the channel type size.
    
    @return     The channel type size.
    */
    uint64_t getChannelTypeSize()    const noexcept;


    /**
    @brief      Gets the channel count.
    
    @return     The channel count. Number between 1 and 4.
    */
    uint32_t getChannelCount()       const noexcept;


    /**
    @brief      Gets the image width in pixels.
    
    @return     The image width in pixels.
    */
    uint32_t getWidth()              const noexcept;


    /**
    @brief      Gets the image height in pixels.
    
    @return     The image height in pixels.
    */
    uint32_t getHeight()             const noexcept;


    /**
    @brief      Gets the image depth in pixels.
    
    @return     The image depth in pixels.
    */
    uint32_t getDepth()              const noexcept;


    /**
    @brief      Creates an image view from this image.
    
    @param[in]  descriptor  The image view descriptor
    
    @return     A new image view object.
    */
    std::shared_ptr<ll::ImageView> createImageView(const ll::ImageViewDescriptor& descriptor);

private:
    Image(const vk::Device& device,
          const vk::Image& vkImage,
          const ll::ImageDescriptor& descriptor,
          const std::shared_ptr<ll::Memory>& memory,
          const ll::MemoryAllocationInfo& allocInfo,
          const vk::ImageLayout layout,
          const vk::ImageUsageFlags usageFlags);

    ll::ImageDescriptor descriptor;
    ll::MemoryAllocationInfo allocInfo;

    vk::Device          device;
    vk::Image           vkImage;
    vk::ImageLayout     vkLayout;
    vk::ImageUsageFlags vkUsageFlags;

    // Shared pointer to the memory this image was created from
    // This will keep the memory alive until this image is deleted
    // avoiding reference to a corrupted memory location.
    std::shared_ptr<ll::Memory> memory;

friend class ll::CommandBuffer;
friend class ll::ComputeNode;
friend class ll::ComputeGraph;
friend class ll::ImageView;
friend class ll::Memory;
friend class ll::Session;
};

} // namespace ll

#endif /* LLUVIA_CORE_IMAGE_H_ */

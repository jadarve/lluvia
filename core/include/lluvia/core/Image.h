/**
@file       Image.h
@brief      Image class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_IMAGE_H_
#define LLUVIA_CORE_IMAGE_H_

#include "lluvia/core/ImageDescriptor.h"
#include "lluvia/core/impl/enum_utils.h"
#include "lluvia/core/MemoryAllocationInfo.h"
#include "lluvia/core/Object.h"
#include "lluvia/core/types.h"

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
        std::make_tuple("TransferSrc"            , vk::ImageUsageFlagBits::eTransferSrc),
        std::make_tuple("TransferDst"            , vk::ImageUsageFlagBits::eTransferDst),
        std::make_tuple("Sampled"                , vk::ImageUsageFlagBits::eSampled),
        std::make_tuple("Storage"                , vk::ImageUsageFlagBits::eStorage),
        std::make_tuple("ColorAttachment"        , vk::ImageUsageFlagBits::eColorAttachment),
        std::make_tuple("DepthStencilAttachment" , vk::ImageUsageFlagBits::eDepthStencilAttachment),
        std::make_tuple("TransientAttachment"    , vk::ImageUsageFlagBits::eTransientAttachment),
        std::make_tuple("InputAttachment"        , vk::ImageUsageFlagBits::eInputAttachment),
    }};


    constexpr const std::array<std::tuple<const char*, vk::ImageLayout>, 13> VkImageLayoutStrings {{
        std::make_tuple("Undefined"                                , vk::ImageLayout::eUndefined),
        std::make_tuple("General"                                  , vk::ImageLayout::eGeneral),
        std::make_tuple("ColorAttachmentOptimal"                   , vk::ImageLayout::eColorAttachmentOptimal),
        std::make_tuple("DepthStencilAttachmentOptimal"            , vk::ImageLayout::eDepthStencilAttachmentOptimal),
        std::make_tuple("DepthStencilReadOnlyOptimal"              , vk::ImageLayout::eDepthStencilReadOnlyOptimal),
        std::make_tuple("ShaderReadOnlyOptimal"                    , vk::ImageLayout::eShaderReadOnlyOptimal),
        std::make_tuple("TransferSrcOptimal"                       , vk::ImageLayout::eTransferSrcOptimal),
        std::make_tuple("TransferDstOptimal"                       , vk::ImageLayout::eTransferDstOptimal),
        std::make_tuple("Preinitialized"                           , vk::ImageLayout::ePreinitialized),
        std::make_tuple("PresentSrcKHR"                            , vk::ImageLayout::ePresentSrcKHR),
        std::make_tuple("SharedPresentKHR"                         , vk::ImageLayout::eSharedPresentKHR),
        std::make_tuple("DepthReadOnlyStencilAttachmentOptimalKHR" , vk::ImageLayout::eDepthReadOnlyStencilAttachmentOptimalKHR),
        std::make_tuple("DepthAttachmentStencilReadOnlyOptimalKHR" , vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimalKHR),
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
inline std::vector<std::string> imageUsageFlagsToVectorString(const vk::ImageUsageFlags flags) noexcept {
    return impl::flagsToVectorString<vk::ImageUsageFlags, vk::ImageUsageFlagBits, impl::VkImageUsageFlagBitsStrings.size(), impl::VkImageUsageFlagBitsStrings>(flags);
}


/**
@brief      Converts from vk::ImageLayout enum value to string

@param[in]  layout  Vulkan image layout enum.

@tparam     T          function return type. Defaults to std::string-

@return     Returns the corresponding `std::string` in ll::impl::VkImageLayoutStrings for the enum value.
*/
template<typename T = std::string>
inline T imageLayoutToString(vk::ImageLayout&& layout) noexcept {
    return impl::enumToString<vk::ImageLayout, ll::impl::VkImageLayoutStrings.size(), ll::impl::VkImageLayoutStrings>(std::forward<vk::ImageLayout>(layout));
}


/**
@brief      Converst from a string-like object to vk::ImageLayout.

This function can be used either with string literals, const char* or `std::string` objects.
\p stringValue parameter is compared against the values in ll::impl::VkImageLayoutStrings and the
corresponding enum value is returned. The comparison is case sensitive.

@param[in]  stringValue  string-like parameter. String literals and `std::string` objects are allowed.

@tparam     T          \p stringValue type. \p T must satisfies `std::is_convertible<T, std::string>()`

@return     vk::ImageLayout value corresponding to stringValue.

@throws std::out_of_range if \p stringValue is not found in ll::impl::VkImageLayoutStrings.
*/
template<typename T>
inline vk::ImageLayout stringToImageLayout(T&& stringValue) {
    return impl::stringToEnum<vk::ImageLayout, T, ll::impl::VkImageLayoutStrings.size(), ll::impl::VkImageLayoutStrings>(std::forward<T>(stringValue));
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
    @brief      Gets the shape of the image.
    
    The vec3ui object returned must be interpreted as follows:

        x : width
        y : height
        z : depth

    @return     The shape.
    */
    ll::vec3ui getShape() const noexcept;

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
          const vk::ImageLayout layout);

    ll::ImageDescriptor descriptor;
    ll::MemoryAllocationInfo allocInfo;

    vk::Device          device;
    vk::Image           vkImage;
    vk::ImageLayout     vkLayout;

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

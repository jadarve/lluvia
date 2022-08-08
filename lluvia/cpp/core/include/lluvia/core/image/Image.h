/**
@file       Image.h
@brief      Image class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_IMAGE_IMAGE_H_
#define LLUVIA_CORE_IMAGE_IMAGE_H_

#include "lluvia/core/image/ImageDescriptor.h"
#include "lluvia/core/image/ImageUsageFlags.h"
#include "lluvia/core/image/ImageLayout.h"
#include "lluvia/core/image/ImageTiling.h"
#include "lluvia/core/memory/MemoryAllocationInfo.h"
#include "lluvia/core/Object.h"
#include "lluvia/core/types.h"

#include "lluvia/core/vulkan/vulkan.hpp"

#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <tuple>
#include <vector>


namespace ll {


namespace vulkan {
class Device;
} // namespace vulkan

class CommandBuffer;
class ComputeNode;
class ComputeGraph;
class ImageView;
class ImageViewDescriptor;
class Memory;
class Session;


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
    @brief      Gets the memory this image was allocated from.
    
    @return     The memory.
    */
    const std::shared_ptr<ll::Memory>& getMemory() const noexcept;


    /**
    @brief      Gets the memory allocation size in bytes.

    This methods is equivalent to calling `getAllocationInfo().size`.
    
    @return     The image size in bytes.
    */
    uint64_t getSize() const noexcept;


    /**
    @brief      Gets the descriptor.
    
    @return     The descriptor.
    */
    const ll::ImageDescriptor& getDescriptor() const noexcept;


    /**
    @brief      Gets the Vulkan image usage flags.
    
    See @VULKAN_DOC#VkImageUsageFlagBits
    for more information.

    @return     The usage flags.
    */
    ll::ImageUsageFlags getUsageFlags() const noexcept;


    /**
    @brief      Gets the usage flags casted to an integer type.

    Please do not use this method. It's for internal use only.
    
    @return     The usage flags unsafe.
    */
    uint32_t getUsageFlagsUnsafe() const noexcept;


    /**
    @brief      Gets the Vulkan image layout.

    The layout is changed through invocations of Session::changeImageLayout
    or CommandBuffer::changeImageLayout.

    See @VULKAN_DOC#_vkimagelayout_3
    
    @return     The image layout.
    */
    ll::ImageLayout     getLayout()     const noexcept;


    /**
    @brief      Gets the vulkan image tiling.
    
    @return     The image tiling.
    */
    ll::ImageTiling getTiling() const noexcept;


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
    
    @tparam     T     Type of the return value. Defaults to ll::ChannelCount.
    
    @return     The channel count.
    */
    template<typename T=ll::ChannelCount>
    T getChannelCount() const noexcept {
        return m_descriptor.getChannelCount<T>();
    }


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
    
    @param[in]  tDescriptor  The image view descriptor
    
    @return     A new image view object.
    */
    std::shared_ptr<ll::ImageView> createImageView(const ll::ImageViewDescriptor& tDescriptor);


    /**
    @brief      Immediately changes the image layout.
    
    This method creates a command buffer and submits it to change
    the layout of the image. Execution is blocked until the layout
    change is completed. 
    
    @param[in]  newLayout  The new layout
    */
    void changeImageLayout(const ll::ImageLayout newLayout);

    /**
    @brief      Immediately clears the image pixels to zero.

    This method creates a command buffer and sumbits it to clear
    all the pixels in the underlying image to zero. Execution is
    blocked until the operation is completed.
    
    */
    void clear();

    /**
    @brief      Immediately copies the content of this image into the destination.

    This method creates a command buffer and sumbits it to copy the content
    of this image into dst. No valiation of destination image shape is performed.
    Execution is blocked until the operation is completed.

    @param[in]  dst  The destination image.
    */
    void copyTo(ll::Image& dst);

private:
    Image(const std::shared_ptr<ll::vulkan::Device>& device,
          const vk::Image& vkImage,
          const ll::ImageDescriptor& descriptor,
          const std::shared_ptr<ll::Memory>& memory,
          const ll::MemoryAllocationInfo& allocInfo,
          const ll::ImageLayout layout);

    std::shared_ptr<ll::vulkan::Device> m_device;

    ll::ImageDescriptor m_descriptor;
    ll::MemoryAllocationInfo m_allocInfo;

    vk::Image           m_vkImage;
    ll::ImageLayout     m_layout;

    // Shared pointer to the memory this image was created from
    // This will keep the memory alive until this image is deleted
    // avoiding reference to a corrupted memory location.
    std::shared_ptr<ll::Memory> m_memory;

friend class ll::CommandBuffer;
friend class ll::ComputeNode;
friend class ll::ComputeGraph;
friend class ll::ImageView;
friend class ll::Memory;
friend class ll::Session;
};

} // namespace ll

#endif /* LLUVIA_CORE_IMAGE_IMAGE_H_ */

/**
@file       ImageView.h
@brief      ImageView class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_IMAGE_IMAGE_VIEW_H_
#define LLUVIA_CORE_IMAGE_IMAGE_VIEW_H_

#include "lluvia/core/Object.h"
#include "lluvia/core/image/Image.h"
#include "lluvia/core/image/ImageDescriptor.h"
#include "lluvia/core/image/ImageViewDescriptor.h"
#include "lluvia/core/memory/MemoryAllocationInfo.h"

#include <memory>

#include "lluvia/core/vulkan/vulkan.hpp"


namespace ll {

namespace vulkan {
class Device;
} // namespace vulkan

class ComputeNode;
class Image;
class Memory;
class Session;


/**
@brief      Represents portions of a ll::Image to be sent as parameter to a GLSL shader.

Image views are created from a ll::Image object provided a ll::ImageViewDescriptor.

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

    // image view descriptor for a sampled image
    auto imgViewDesc = ll::ImageViewDescriptor {}
                        .setNormalizedCoordinates(false)
                        .setIsSampled(true)
                        .setAddressMode(ll::ImageAddressMode::Repeat)
                        .setFilterMode(ll::ImageFilterMode::Nearest);

    auto imageView = image->createImageView(imgViewDesc);
@endcode

ImageView objects are mapped to GLSL objects as follows:

- **GLSL `image`** object if the ImageView was created with a ImageViewDescriptor with `setIsSampled(false)`.
    For this case, image load/store operations should be used to access pixel data.
    See https://www.khronos.org/opengl/wiki/Image_Load_Store for more information.

    For instance, an ImageView created as
    @code

        // 8-bit unsigned, 4-channel, 640x480 image.
        auto desc = ll::ImageDescriptor{}
                        .setWidth(640)
                        .setHeight(480)
                        .setChannelType(ll::ChannelType::Uint8)
                        .setChannelCount(4);

        auto image = memory->createImage(desc);

        // image view descriptor for a sampled image
        auto imgViewDesc = ll::ImageViewDescriptor {}
                            .setNormalizedCoordinates(false)
                            .setIsSampled(false);

        auto imageView = image->createImageView(imgViewDesc);
    @endcode

    is accessed in a GLSL compute shader as
    @code
        #version 450

        #include "lluvia/core.glsl"

        layout(binding = 0, rgba8ui) uniform uimage2D inputImage;

        const uint OUTPUT_WIDTH = 32;

        void main() {

            const ivec2 coord = LL_GLOBAL_COORDS_2D;
            uvec4 texValue = imageLoad(inputImage, coord);
        }

    @endcode

- **GLSL `sampler`** object if the ImageView was created with a ImageDescriptor with `setIsSampled(true)`.
    In this case texture lookup and write operations should be used.
    See https://www.khronos.org/opengl/wiki/Sampler_(GLSL) for more information.

    For instance, an ImageView created as
    @code

        // 8-bit unsigned, 4-channel, 640x480 image.
        auto desc = ll::ImageDescriptor{}
                        .setWidth(640)
                        .setHeight(480)
                        .setChannelType(ll::ChannelType::Uint8)
                        .setChannelCount(4);

        auto image = memory->createImage(desc);

        // image view descriptor for a sampled image
        auto imgViewDesc = ll::ImageViewDescriptor {}
                        .setNormalizedCoordinates(false)
                        .setIsSampled(true)
                        .setAddressMode(ll::ImageAddressMode::Repeat)
                        .setFilterMode(ll::ImageFilterMode::Nearest);

        auto imageView = image->createImageView(imgViewDesc);
    @endcode

    is accessed in a GLSL compute shader as
    @code
        #version 450

        #include "lluvia/core.glsl"

        layout(binding = 0) uniform usampler2D img;


        void main() {

            const ivec2 coord = LL_GLOBAL_COORDS_2D;
            uvec4 texValue = texture(img, coord);
        }
    @endcode

*/
class ImageView : public Object {

public:
    ImageView()                    = delete;
    ImageView(const ImageView&)    = delete;
    ImageView(ImageView&&)         = delete;

    ~ImageView();

    ImageView& operator = (const ImageView&) = delete;
    ImageView& operator = (ImageView&&)      = delete;

    ll::ObjectType getType() const noexcept override;


    /**
    @brief      Gets the parent ll::Image this object was created from.
    
    @return     The parent ll::Image object.
    */
    const std::shared_ptr<ll::Image>& getImage() const noexcept;


    /**
    @brief      Gets the memory the underlying ll::Image was allocated from.
    
    @return     The memory.
    */
    const std::shared_ptr<ll::Memory>& getMemory() const noexcept;


    /**
    @brief      Gets the allocation information of the underlying ll::Image object.
    
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
    @brief      Gets the image descriptor.
    
    @return     The image descriptor.
    */
    const ll::ImageDescriptor& getImageDescriptor() const noexcept;


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
    ll::ImageLayout getLayout() const noexcept;


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
    uint64_t getChannelTypeSize() const noexcept;


    /**
    @brief      Gets the channel count.
    
    @tparam     T     Type of the return value. Defaults to ll::ChannelCount.
    
    @return     The channel count.
    */
    template<typename T=ll::ChannelCount>
    T getChannelCount() const noexcept {
        return this->m_image->getChannelCount<T>();
    }


    /**
    @brief      Gets the image view  width in pixels.
    
    @return     The image view width in pixels.
    */
    uint32_t getWidth() const noexcept;


    /**
    @brief      Gets the image view height in pixels.
    
    @return     The image view height in pixels.
    */
    uint32_t getHeight() const noexcept;


    /**
    @brief      Gets the image view depth in pixels.
    
    @return     The image view depth in pixels.
    */
    uint32_t getDepth() const noexcept;

    /**
    @brief      Gets the shape of the image view.
    
    The vec3ui object returned must be interpreted as follows:

        x : width
        y : height
        z : depth

    @return     The shape.
    */
    ll::vec3ui getShape() const noexcept;


    /**
    @brief      Gets the image view descriptor.
    
    @return     The descriptor.
    */
    const ll::ImageViewDescriptor& getDescriptor() const noexcept;


    /**
    @brief      Immediately changes the layout of the underlying ll::Image object.
    
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
    @brief      Immediately copies the content of this image view into the destination.

    This method creates a command buffer and sumbits it to copy the content
    of this image view underlying Image object into dst Image object.
    No valiation of destination image shape is performed.
    Execution is blocked until the operation is completed.
    
    @param[in]  dst  The destination image view..
    */
    void copyTo(ll::ImageView& dst);

private:
    ImageView(const std::shared_ptr<ll::vulkan::Device>& device,
              const std::shared_ptr<ll::Image>& image,
              const ll::ImageViewDescriptor& descriptor);
    
    ll::ImageViewDescriptor m_descriptor;

    
    vk::ImageView m_vkImageView;
    vk::Sampler   m_vkSampler;

    std::shared_ptr<ll::vulkan::Device> m_device;
    std::shared_ptr<ll::Image>          m_image;


friend class Image;
friend class ComputeNode;
};

} // namespace ll

#endif /* LLUVIA_CORE_IMAGE_IMAGE_VIEW_H_ */

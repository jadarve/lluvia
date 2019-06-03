/**
@file       ImageView.h
@brief      ImageView class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_IMAGE_VIEW_H_
#define LLUVIA_CORE_IMAGE_VIEW_H_

#include "lluvia/core/Object.h"
#include "lluvia/core/Image.h"
#include "lluvia/core/ImageDescriptor.h"
#include "lluvia/core/ImageViewDescriptor.h"
#include "lluvia/core/MemoryAllocationInfo.h"

#include <memory>

#include <vulkan/vulkan.hpp>


namespace ll {

class Image;


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
    std::shared_ptr<ll::Image> getImage() const noexcept;


    /**
    @brief      Gets the allocation information of the underlying ll::Image object.
    
    @return     The allocation information.
    */
    inline ll::MemoryAllocationInfo getAllocationInfo() const noexcept {
        return this->image->getAllocationInfo();
    }


    /**
    @brief      Gets the memory allocation size in bytes.

    This methods is equivalent to calling `getAllocationInfo().size`.
    
    @return     The image size in bytes.
    */
    inline uint64_t getSize() const noexcept {
        return this->image->getSize();
    }


    /**
    @brief      Gets the Vulkan image usage flags.
    
    See @VULKAN_DOC#VkImageUsageFlagBits
    for more information.

    @return     The usage flags.
    */
    inline vk::ImageUsageFlags getUsageFlags() const noexcept {
        return this->image->getUsageFlags();
    }


    /**
    @brief      Gets the Vulkan image layout.

    The layout is changed through invocations of Session::changeImageLayout
    or CommandBuffer::changeImageLayout.

    See @VULKAN_DOC#_vkimagelayout_3
    
    @return     The image layout.
    */
    inline vk::ImageLayout getLayout() const noexcept {
        return this->image->getLayout();
    }


    /**
    @brief      Gets the channel type.
    
    @return     The channel type.
    */
    inline ll::ChannelType getChannelType() const noexcept {
        return this->image->getChannelType();
    }


    /**
    @brief      Gets the channel type size.
    
    @return     The channel type size.
    */
    inline uint64_t getChannelTypeSize() const noexcept {
        return this->image->getChannelTypeSize();
    }


    /**
    @brief      Gets the channel count.
    
    @tparam     T     Type of the return value. Defaults to ll::ChannelCount.
    
    @return     The channel count.
    */
    template<typename T=ll::ChannelCount>
    T getChannelCount() const noexcept {
        return this->image->getChannelCount<T>();
    }


    /**
    @brief      Gets the image view  width in pixels.
    
    @return     The image view width in pixels.
    */
    inline uint32_t getWidth() const noexcept {
        return this->image->getWidth();
    }


    /**
    @brief      Gets the image view height in pixels.
    
    @return     The image view height in pixels.
    */
    inline uint32_t getHeight() const noexcept {
        return this->image->getHeight();
    }


    /**
    @brief      Gets the image view depth in pixels.
    
    @return     The image view depth in pixels.
    */
    inline uint32_t getDepth() const noexcept {
        return this->image->getDepth();
    }

    /**
    @brief      Gets the shape of the image view.
    
    The vec3ui object returned must be interpreted as follows:

        x : width
        y : height
        z : depth

    @return     The shape.
    */
    inline ll::vec3ui getShape() const noexcept {
        return this->image->getShape();
    }


    /**
    @brief      Gets the image view descriptor.
    
    @return     The descriptor.
    */
    inline const ll::ImageViewDescriptor& getDescriptor() const noexcept {
        return descriptor;
    }

private:
    ImageView(  vk::Device tDevice,
                std::shared_ptr<ll::Image> tImage,
                const ll::ImageViewDescriptor& tDescriptor);
    
    ll::ImageViewDescriptor descriptor;

    vk::Device    device;
    vk::ImageView vkImageView;
    vk::Sampler   vkSampler;

    std::shared_ptr<ll::Image> image;


friend class Image;
friend class ComputeNode;
};

} // namespace ll

#endif /* LLUVIA_CORE_IMAGE_VIEW_H_ */

/**
@file       CommandBuffer.h
@brief      CommandBuffer class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_COMMAND_BUFFER_H_
#define LLUVIA_CORE_COMMAND_BUFFER_H_

#include <vulkan/vulkan.hpp>


namespace ll {

class Buffer;
class ComputeNode;
class Duration;
class Image;
class ImageView;
class Session;


/**
@brief      Class for command buffer.

A command buffer object records operations to be submitted to the Vulkan device. Such
operations include running a ll::ComputeNode, copying ll::Buffer objects, changing
layout of ll::Image objects or creating memory barriers.

ll::CommandBuffers are created through ll::Session objects.

@code
    auto session = ll::Session::create();
    
    auto cmdBuffer = session->createCommandBuffer();
    cmdBuffer->begin();

        cmdBuffer->changeImageLayout(*image, vk::ImageLayout::eTransferDstOptimal);
        cmdBuffer->copyBufferToImage(*stageBuffer, *image);
        cmdBuffer->changeImageLayout(*image, vk::ImageLayout::eGeneral);

        // bind parameters to a ll::ComputeNode object.
        // the image view can only be bound after the underlying
        // image is in the correct layout.
        node->bind(0, imageView);
        node->bind(1, outputBuffer);

        cmdBuffer->run(*node);

    // finish recording and submit for execution
    cmdBuffer->end();
    session->run(cmdBuffer);
@endcode
*/
class CommandBuffer {

public:
    CommandBuffer()                                 = delete;
    CommandBuffer(const CommandBuffer& cmdBuffer)   = delete;
    CommandBuffer(CommandBuffer&& cmdBuffer)        = delete;

    CommandBuffer(const vk::Device& tDevice, const vk::CommandPool& cmdPool);

    ~CommandBuffer();

    CommandBuffer& operator = (const CommandBuffer& cmdBuffer) = delete;
    CommandBuffer& operator = (CommandBuffer&& cmdBuffer)      = delete;


    const vk::CommandBuffer& getVkCommandBuffer() const noexcept;

    /**
    @brief      begins recording.

    This method should be called before any record operation. Otherwise
    the behaviour is undefined.
    */
    void begin();


    /**
    @brief      ends recording.

    Any following call to recording methods after this call have undefined
    behavior.
    */
    void end();


    /**
    @brief      Records running a ll::ComputeNode
    
    @param[in]  node  The node
    */
    void run(const ll::ComputeNode& node);


    /**
    @brief      Copies \p src buffer into \p dst.

    The parameters must satisfy `dst.getSize() >= src.getSize()`.
    
    @param[in]  src   The source buffer.
    @param[in]  dst   The destination buffer.

    @throws     std::system_erro with error code ll::ErrorCode::BufferCopyError if `dst.getSize() < src.getSize()`.
    */
    void copyBuffer(const ll::Buffer& src, const ll::Buffer& dst);


    /**
    @brief      Copies the content of \p src buffer into \p dst image.

    **TODO:** assert image size.
    
    @param[in]  src   The source
    @param[in]  dst   The destination
    */
    void copyBufferToImage(const ll::Buffer& src, const ll::Image& dst);


    /**
    @brief      Copies the content of \p src image into \p dst buffer.

    *TODO:* assert buffer size.
    
    @param[in]  src   The source
    @param[in]  dst   The destination
    */
    void copyImageToBuffer(const ll::Image& src, const ll::Buffer& dst);


    /**
    @brief      Copies the content of \p src image into \p dst image.

    *TODO:* assert buffer size.
    
    @param[in]  src   The source
    @param[in]  dst   The destination
    */
    void copyImageToImage(const ll::Image& src, const ll::Image& dst);


    /**
    @brief      Change \p image layout.

    The actual change in image layout is performed when this command buffer
    is submitted to execution. However, calling `image.getLayout()` right after
    this call will return the new layout value.

    This is necessary to keep track of the current image layout of \p image parameter
    after this call.
    
    @param      image      The image
    @param[in]  newLayout  The new layout
    */
    void changeImageLayout(ll::Image& image, const vk::ImageLayout newLayout);

    /**
    @brief      Change \p imageView underlying image layout.

    The actual change in image layout is performed when this command buffer
    is submitted to execution. However, calling `imageView.getImage()->getLayout()` right after
    this call will return the new layout value.

    This is necessary to keep track of the current image layout of \p image parameter
    after this call.
    
    @param      imageView  The image view.
    @param[in]  newLayout  The new layout
    */
    void changeImageLayout(ll::ImageView& imageView, const vk::ImageLayout newLayout);


    /**
    @brief      Adds a memory barrier.

    *TODO:* details.
    */
    void memoryBarrier();

    /**
    @brief      Clears the pixels of an image to zero.
    */
    void clearImage(ll::Image& image);

    /**
    @brief      Starts recording the elapsed time between two points.
    
    @param      duration  The duration object.
    */
    void durationStart(ll::Duration& duration);

    /**
    @brief      Stops recording the elapsed time between two points.
    
    @param      duration  The duration object.
    */
    void durationEnd(ll::Duration& duration);

private:
    vk::Device          m_device;
    vk::CommandPool     m_commandPool;
    vk::CommandBuffer   m_commandBuffer;


friend class ll::Session;
};


} // namespace ll

#endif // LLUVIA_CORE_COMMAND_BUFFER_H_

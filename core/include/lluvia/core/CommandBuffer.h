#ifndef LLUVIA_CORE_COMMAND_BUFFER_H_
#define LLUVIA_CORE_COMMAND_BUFFER_H_

#include <vulkan/vulkan.hpp>


namespace ll {

class Buffer;
class ComputeNode;
class Image;
class Session;

class CommandBuffer {

public:
    CommandBuffer()                                 = delete;
    CommandBuffer(const CommandBuffer& cmdBuffer)   = delete;
    CommandBuffer(CommandBuffer&& cmdBuffer)        = delete;

    CommandBuffer(const vk::Device& device, const vk::CommandPool& cmdPool);

    ~CommandBuffer();

    CommandBuffer& operator = (const CommandBuffer& cmdBuffer) = delete;
    CommandBuffer& operator = (CommandBuffer&& cmdBuffer)      = delete;

    void begin();
    void end();

    void run(const ll::ComputeNode& node);
    void copyBuffer(const ll::Buffer& src, const ll::Buffer& dst);
    void copyBufferToImage(const ll::Buffer& src, const ll::Image& dst);
    void copyImageToBuffer(const ll::Image& src, const ll::Buffer& dst);
    void changeImageLayout(ll::Image& image, const vk::ImageLayout newLayout);
    void memoryBarrier();

private:
    vk::Device          device;
    vk::CommandPool     commandPool;
    vk::CommandBuffer   commandBuffer;


friend class ll::Session;
};


} // namespace ll

#endif // LLUVIA_CORE_COMMAND_BUFFER_H_

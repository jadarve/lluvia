#ifndef LLUVIA_CORE_COMMAND_BUFFER_H_
#define LLUVIA_CORE_COMMAND_BUFFER_H_


#include <vulkan/vulkan.hpp>


namespace ll {


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

private:
    vk::Device          device;
    vk::CommandPool     commandPool;
    vk::CommandBuffer   commandBuffer;
};


} // namespace ll

#endif // LLUVIA_CORE_COMMAND_BUFFER_H_

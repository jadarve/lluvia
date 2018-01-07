#ifndef LLUVIA_CORE_IMPL_ONE_TIME_SUBMIT_COMMAND_BUFFER_H_
#define LLUVIA_CORE_IMPL_ONE_TIME_SUBMIT_COMMAND_BUFFER_H_

#include <vulkan/vulkan.hpp>

namespace ll {
namespace impl {

class OneTimeSubmitCommandBuffer {

public:
    OneTimeSubmitCommandBuffer()                                     = delete;
    OneTimeSubmitCommandBuffer(const OneTimeSubmitCommandBuffer&) = delete;
    OneTimeSubmitCommandBuffer(OneTimeSubmitCommandBuffer&&)      = delete;

    OneTimeSubmitCommandBuffer(  const vk::Device& device,
                                    uint32_t queueFamilyIndex);

    ~OneTimeSubmitCommandBuffer();

    OneTimeSubmitCommandBuffer& operator = (const OneTimeSubmitCommandBuffer&) = delete;
    OneTimeSubmitCommandBuffer& operator = (OneTimeSubmitCommandBuffer&&)      = delete;

    inline const vk::CommandBuffer& getCommandBuffer() const noexcept {return commandBuffer;};
    void runAndWait(const vk::Queue& queue);

private:
    vk::Device device;
    vk::CommandPool commandPool;
    vk::CommandBuffer commandBuffer;
};

} // namespace impl
} // namespace ll


#endif // LLUVIA_CORE_IMPL_ONE_TIME_SUBMIT_COMMAND_BUFFER_H_
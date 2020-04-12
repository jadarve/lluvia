#include "lluvia/core/vulkan/CommandPool.h"
#include "lluvia/core/vulkan/Device.h"

namespace ll {
namespace vulkan {

CommandPool::CommandPool(const vk::CommandPool& commandPool, const std::shared_ptr<ll::vulkan::Device>& device) :
    m_commandPool{commandPool},
    m_device{device}
{
}

CommandPool::~CommandPool() {
    m_device->get().destroyCommandPool(m_commandPool);
}

vk::CommandPool &CommandPool::get() noexcept {
    return m_commandPool;
}

} // namespace vulkan
} // namespace ll

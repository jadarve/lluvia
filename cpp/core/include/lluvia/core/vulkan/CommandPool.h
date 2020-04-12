/**
@file       CommandPool.h
@brief      CommandPool class.
@copyright  2020, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_VULKAN_COMMAND_POOL_H_
#define LLUVIA_CORE_VULKAN_COMMAND_POOL_H_

#include <memory>

#include <vulkan/vulkan.hpp>

namespace ll {
namespace vulkan {

class Device;

class CommandPool {

public:
    CommandPool() = delete;
    CommandPool(const CommandPool& CommandPool) = delete;
    CommandPool(CommandPool&& CommandPool) = delete;

    CommandPool(const vk::CommandPool& CommandPool, const std::shared_ptr<ll::vulkan::Device>& device);
    ~CommandPool();

    CommandPool &operator=(const CommandPool &CommandPool) = delete;
    CommandPool &operator=(CommandPool &&CommandPool) = delete;

    vk::CommandPool &get() noexcept;

private:
    vk::CommandPool m_commandPool;
    std::shared_ptr<ll::vulkan::Device> m_device;
};

} // namespace vulkan
} // namespace ll

#endif // LLUVIA_CORE_VULKAN_COMMAND_POOL_H_

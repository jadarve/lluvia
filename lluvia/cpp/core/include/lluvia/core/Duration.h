/**
@file       Duration.h
@brief      Duration class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_DURATION_H_
#define LLUVIA_CORE_DURATION_H_

#include <chrono>
#include <memory>

#include "lluvia/core/vulkan/vulkan.hpp"

namespace ll {

namespace vulkan {
    class Device;
} // namespace vulkan

class Duration {

public:
    Duration() = delete;
    Duration(const Duration& duration) = delete;
    Duration(Duration&& duration) = delete;

    Duration(const std::shared_ptr<ll::vulkan::Device> device);

    ~Duration();

    Duration& operator=(const Duration& duration) = delete;
    Duration& operator=(Duration&& duration) = delete;

    std::chrono::nanoseconds getDuration() const;

    int64_t getNanoseconds() const;

    vk::QueryPool getQueryPool() const noexcept;

    uint32_t getStartTimeQueryIndex() const noexcept;
    uint32_t getEndTimeQueryIndex() const noexcept;

private:
    vk::QueryPool m_queryPool;

    std::shared_ptr<ll::vulkan::Device> m_device;
};

} // namespace ll

#endif // LLUVIA_CORE_DURATION_H_

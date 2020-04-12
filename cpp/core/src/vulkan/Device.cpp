#include "lluvia/core/vulkan/Device.h"

namespace ll {
namespace vulkan {

Device::Device(const vk::Device &device, const std::shared_ptr<ll::vulkan::Instance> &instance):
    m_device {device},
    m_instance {instance} {

}


Device::~Device() {
    m_device.destroy();
}


vk::Device &Device::get() noexcept {
    return m_device;
}

} // namespace vulkan
} // namespace ll

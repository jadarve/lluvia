#include "lluvia/core/vulkan/Instance.h"

namespace ll {
namespace vulkan {

Instance::Instance(const vk::Instance &instance): 
    m_instance{instance} {

}


Instance::~Instance() {
    m_instance.destroy();
}


vk::Instance& Instance::get() noexcept {
    return m_instance;
}

} // namespace vulkan
} // namespace ll

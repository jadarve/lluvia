#include "lluvia/core/Shader.h"

namespace ll {

Shader::Shader(const vk::Device& device, const std::vector<char>& spirvCode):
    device {device} {

    vk::ShaderModuleCreateInfo moduleCreateInfo = vk::ShaderModuleCreateInfo()
            .setCodeSize(spirvCode.size())
            .setPCode(reinterpret_cast<const uint32_t*>(spirvCode.data()));

    module = device.createShaderModule(moduleCreateInfo);
}

Shader::~Shader() {
    device.destroyShaderModule(module);
}

} // namespace ll
#include "lluvia/core/Shader.h"

namespace ll {

Shader::Shader(const vk::Device& device, const std::vector<char>& spirvCode):
    device {device},
    referenceCounter {std::make_shared<int>(0)} {

    vk::ShaderModuleCreateInfo moduleCreateInfo = vk::ShaderModuleCreateInfo()
            .setCodeSize(spirvCode.size())
            .setPCode(reinterpret_cast<const uint32_t*>(spirvCode.data()));

    module = device.createShaderModule(moduleCreateInfo);
}

Shader::~Shader() {

    if (referenceCounter.use_count() == 1) {

        device.destroyShaderModule(module);
    }
}

} // namespace ll
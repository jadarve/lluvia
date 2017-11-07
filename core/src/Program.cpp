#include "lluvia/core/Program.h"

namespace ll {

Program::Program(const vk::Device& device, const std::vector<char>& spirvCode):
    device {device} {

    vk::ShaderModuleCreateInfo moduleCreateInfo = vk::ShaderModuleCreateInfo()
            .setCodeSize(spirvCode.size())
            .setPCode(reinterpret_cast<const uint32_t*>(spirvCode.data()));

    module = device.createShaderModule(moduleCreateInfo);
}

Program::~Program() {
    device.destroyShaderModule(module);
}

vk::ShaderModule Program::getShaderModule() const noexcept {
    return module;
}

} // namespace ll
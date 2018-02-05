#include "lluvia/core/Program.h"
#include "lluvia/core/Visitor.h"

namespace ll {

Program::Program(const vk::Device& device, const std::vector<uint8_t>& spirvCode):
    device    {device},
    spirvCode {spirvCode} {

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


const std::vector<uint8_t>& Program::getSpirV() const noexcept {
    return spirvCode;
}


void Program::accept(ll::Visitor* visitor) {
    assert(visitor != nullptr);
}

} // namespace ll
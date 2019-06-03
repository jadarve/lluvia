/**
@file       Program.cpp
@brief      Program class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#include "lluvia/core/error.h"
#include "lluvia/core/Program.h"
#include "lluvia/core/Visitor.h"

namespace ll {

Program::Program(
    const std::shared_ptr<const ll::Session>& tSession,
    const vk::Device& tDevice,
    const std::vector<uint8_t>& tSpirvCode):

    device    {tDevice},
    spirvCode {tSpirvCode},
    session   {tSession} {

    if (spirvCode.empty()) {
        throw std::system_error(createErrorCode(ll::ErrorCode::ProgramCompilationError), "Zero size SPIR-V code.");
    }

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


void Program::accept(__attribute__((unused)) ll::Visitor* visitor) {
    
}

} // namespace ll
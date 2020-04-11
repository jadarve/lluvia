/**
@file       Program.cpp
@brief      Program class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#include "lluvia/core/error.h"
#include "lluvia/core/Program.h"

namespace ll {

Program::Program(
    const std::shared_ptr<const ll::Session>& session,
    const vk::Device& device,
    const std::vector<uint8_t>& spirvCode):

    m_device    {device},
    m_spirvCode {spirvCode},
    m_session   {session} {

    ll::throwSystemErrorIf(m_spirvCode.empty(), ll::ErrorCode::ProgramCompilationError, "Zero size SPIR-V code.");

    vk::ShaderModuleCreateInfo moduleCreateInfo = vk::ShaderModuleCreateInfo()
            .setCodeSize(m_spirvCode.size())
            .setPCode(reinterpret_cast<const uint32_t*>(m_spirvCode.data()));

    m_module = m_device.createShaderModule(moduleCreateInfo);
}


Program::~Program() {
    m_device.destroyShaderModule(m_module);
}


vk::ShaderModule Program::getShaderModule() const noexcept {
    return m_module;
}


const std::vector<uint8_t>& Program::getSpirV() const noexcept {
    return m_spirvCode;
}

} // namespace ll

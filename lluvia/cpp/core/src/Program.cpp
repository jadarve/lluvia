/**
@file       Program.cpp
@brief      Program class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#include "lluvia/core/Program.h"
#include "lluvia/core/error.h"

#include "lluvia/core/vulkan/Device.h"

namespace ll {

Program::Program(
    const std::shared_ptr<ll::vulkan::Device>& device,
    const std::vector<uint8_t>&                spirvCode)
    :

    m_device {device}
    , m_spirvCode {spirvCode}
{

    ll::throwSystemErrorIf(m_spirvCode.empty(), ll::ErrorCode::ProgramCompilationError, "Zero size SPIR-V code.");

    vk::ShaderModuleCreateInfo moduleCreateInfo = vk::ShaderModuleCreateInfo()
                                                      .setCodeSize(m_spirvCode.size())
                                                      .setPCode(reinterpret_cast<const uint32_t*>(m_spirvCode.data()));

    m_module = m_device->get().createShaderModule(moduleCreateInfo);
}

Program::~Program()
{
    m_device->get().destroyShaderModule(m_module);
}

vk::ShaderModule Program::getShaderModule() const noexcept
{
    return m_module;
}

const std::vector<uint8_t>& Program::getSpirV() const noexcept
{
    return m_spirvCode;
}

} // namespace ll

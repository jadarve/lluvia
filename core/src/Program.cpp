/**
@file       Program.cpp
@brief      Program class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#include "lluvia/core/Program.h"
#include "lluvia/core/Visitor.h"

namespace ll {

Program::Program(const std::shared_ptr<const ll::Session>& session, const vk::Device& device, const std::vector<uint8_t>& spirvCode):
    device    {device},
    spirvCode {spirvCode},
    session   {session} {

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
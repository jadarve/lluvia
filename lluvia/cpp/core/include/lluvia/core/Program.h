/**
@file       Program.h
@brief      Program class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_PROGRAM_H_
#define LLUVIA_CORE_PROGRAM_H_

#include <memory>
#include <vector>

#include "lluvia/core/vulkan/vulkan.hpp"

namespace ll {

namespace vulkan {
    class Device;
} // namespace vulkan

// forward declarations
class Session;

/**
@brief      Class representing Vulkan shader modules in SPIR-V representation.

Program objects hold the SPIR-V code needed to run shader programs in the device. They
are created using Session::createProgram methods.

@code
    auto session = ll::Session::create();

    auto program = session->createProgram("path_to_spir-v.spv");
@endcode
*/
class Program {

public:
    Program()                       = delete;
    Program(const Program& program) = delete;
    Program(Program&& program)      = delete;

    /**
    @brief      Constructs the object from a Vulkan device and SPIR-V code.

    @param[in]  session    The session this node was created from.
    @param[in]  device     The Vulkan device.
    @param[in]  spirvCode  The SPIR-V code.
    */
    Program(
        const std::shared_ptr<ll::vulkan::Device>& device,
        const std::vector<uint8_t>&                spirvCode);

    ~Program();

    Program& operator=(const Program& program) = delete;
    Program& operator=(Program&& program)      = delete;

    /**
    @brief      Gets the corresponding Vulkan shader module for this program object.

    @return     The shader module.
    */
    vk::ShaderModule getShaderModule() const noexcept;

    /**
    @brief      Gets the SPIR-V code of this object.

    @return     The SPIR-V code.
    */
    const std::vector<uint8_t>& getSpirV() const noexcept;

private:
    std::shared_ptr<ll::vulkan::Device> m_device;

    vk::ShaderModule     m_module;
    std::vector<uint8_t> m_spirvCode;
};

} // namespace ll

#endif // LLUVIA_CORE_PROGRAM_H_

#ifndef LLUVIA_CORE_PROGRAM_H_
#define LLUVIA_CORE_PROGRAM_H_

#include <memory>
#include <vector>

#include <vulkan/vulkan.hpp>


namespace ll {

class Program {

public:
    Program()                          = delete;
    Program(const Program& program)    = delete;
    Program(Program&& program)         = delete;
    Program(const vk::Device& device, const std::vector<char>& spirvCode);

    ~Program();

    Program& operator = (const Program& program) = delete;
    Program& operator = (Program&& program)      = delete;

    vk::ShaderModule getShaderModule() const noexcept;

private:
    vk::Device       device;
    vk::ShaderModule module;
};

} // namespace ll

#endif // LLUVIA_CORE_PROGRAM_H_

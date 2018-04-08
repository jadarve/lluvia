#ifndef LLUVIA_CORE_PROGRAM_H_
#define LLUVIA_CORE_PROGRAM_H_

#include <memory>
#include <vector>

#include <vulkan/vulkan.hpp>


namespace ll {

// forward declarations
class Visitor;


class Program {

public:
    Program()                          = delete;
    Program(const Program& program)    = delete;
    Program(Program&& program)         = delete;
    Program(const vk::Device& device, const std::vector<uint8_t>& spirvCode);

    ~Program();

    Program& operator = (const Program& program) = delete;
    Program& operator = (Program&& program)      = delete;

    vk::ShaderModule getShaderModule() const noexcept;

    const std::vector<uint8_t>& getSpirV() const noexcept;

    void accept(ll::Visitor* visitor);

private:
    vk::Device           device;
    vk::ShaderModule     module;
    std::vector<uint8_t> spirvCode;
};

} // namespace ll

#endif // LLUVIA_CORE_PROGRAM_H_

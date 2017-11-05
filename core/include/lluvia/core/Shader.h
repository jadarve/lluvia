#ifndef LLUVIA_CORE_SHADER_H_
#define LLUVIA_CORE_SHADER_H_

#include <memory>
#include <vector>

#include <vulkan/vulkan.hpp>


namespace ll {

class Shader {

public:
    Shader()                        = delete;
    Shader(const Shader& shader)    = delete;
    Shader(Shader&& shader)         = delete;
    Shader(const vk::Device& device, const std::vector<char>& spirvCode);

    ~Shader();

    Shader& operator = (const Shader& shader) = delete;
    Shader& operator = (Shader&& shader)      = delete;


private:
    vk::Device       device;
    vk::ShaderModule module;
};

} // namespace ll

#endif // LLUVIA_CORE_SHADER_H_

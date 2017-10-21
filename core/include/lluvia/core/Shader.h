#ifndef LLUVIA_CORE_SHADER_H_
#define LLUVIA_CORE_SHADER_H_

#include <memory>
#include <vector>

#include <vulkan/vulkan.hpp>


namespace ll {

class Shader {

public:
    Shader()                        = default;
    Shader(const Shader& shader)    = default;
    Shader(Shader&& shader)         = default;
    Shader(const vk::Device& device, const std::vector<char>& spirvCode);

    ~Shader();

    Shader& operator = (const Shader& shader) = default;
    Shader& operator = (Shader&& shader)      = default;


private:
    vk::Device device;
    vk::ShaderModule module;

    std::shared_ptr<int> referenceCounter {nullptr};
};

} // namespace ll

#endif // LLUVIA_CORE_SHADER_H_

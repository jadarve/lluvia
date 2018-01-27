#ifndef LLUVIA_CORE_UTILS_H_
#define LLUVIA_CORE_UTILS_H_

#include <vulkan/vulkan.hpp>

#include <string>
#include <vector>


namespace ll {

std::vector<std::string> memoryPropertyFlagsToVectorString(const vk::MemoryPropertyFlags flags);

std::vector<std::string> bufferUsageFlagsToVectorString(const vk::BufferUsageFlags flags);

} // namespace ll

#endif // LLUVIA_CORE_UTILS_H_

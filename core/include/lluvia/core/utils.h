#ifndef LLUVIA_CORE_UTILS_H_
#define LLUVIA_CORE_UTILS_H_

#include <vulkan/vulkan.hpp>

#include <array>
#include <cstdint>
#include <string>
#include <vector>


namespace ll {

vk::MemoryPropertyFlags vectorStringToMemoryPropertyFlags(const std::vector<std::string>& flagsVector);
std::vector<std::string> memoryPropertyFlagsToVectorString(const vk::MemoryPropertyFlags flags);

vk::BufferUsageFlags vectorStringToBufferUsageFLags(const std::vector<std::string>& flagsVector);
std::vector<std::string> bufferUsageFlagsToVectorString(const vk::BufferUsageFlags flags);


std::string toBase64(const void* ptr, const size_t size);
std::vector<uint8_t> fromBase64(const std::string& code);

} // namespace ll

#endif // LLUVIA_CORE_UTILS_H_

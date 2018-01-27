#include "lluvia/core/utils.h"


namespace ll {


constexpr const char* STRING_VK_MEMORY_DEVICE_LOCAL     = "DEVICE_LOCAL";
constexpr const char* STRING_VK_MEMORY_HOST_VISIBLE     = "HOST_VISIBLE";
constexpr const char* STRING_VK_MEMORY_HOST_COHERENT    = "HOST_COHERENT";
constexpr const char* STRING_VK_MEMORY_HOST_CACHED      = "HOST_CACHED";
constexpr const char* STRING_VK_MEMORY_LAZILY_ALLOCATED = "LAZILY_ALLOCATED";


constexpr const char* STRING_VK_BUFFER_USAGE_TRANSFER_SRC         = "TRANSFER_SRC";
constexpr const char* STRING_VK_BUFFER_USAGE_TRANSFER_DST         = "TRANSFER_DST";
constexpr const char* STRING_VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER = "UNIFORM_TEXEL_BUFFER";
constexpr const char* STRING_VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER = "STORAGE_TEXEL_BUFFER";
constexpr const char* STRING_VK_BUFFER_USAGE_UNIFORM_BUFFER       = "UNIFORM_BUFFER";
constexpr const char* STRING_VK_BUFFER_USAGE_STORAGE_BUFFER       = "STORAGE_BUFFER";
constexpr const char* STRING_VK_BUFFER_USAGE_INDEX_BUFFER         = "INDEX_BUFFER";
constexpr const char* STRING_VK_BUFFER_USAGE_VERTEX_BUFFER        = "VERTEX_BUFFER";
constexpr const char* STRING_VK_BUFFER_USAGE_INDIRECT_BUFFER      = "INDIRECT_BUFFE";


auto compareFlagBit = [](const auto& flags, const auto& value) {
    return ((flags & value) == value);
};

std::vector<std::string> memoryPropertyFlagsToVectorString(const vk::MemoryPropertyFlags flags) {


    auto flagsVector = std::vector<std::string> {};

    if (compareFlagBit(flags, vk::MemoryPropertyFlagBits::eDeviceLocal))     flagsVector.push_back(STRING_VK_MEMORY_DEVICE_LOCAL);
    if (compareFlagBit(flags, vk::MemoryPropertyFlagBits::eHostVisible))     flagsVector.push_back(STRING_VK_MEMORY_HOST_VISIBLE);
    if (compareFlagBit(flags, vk::MemoryPropertyFlagBits::eHostCoherent))    flagsVector.push_back(STRING_VK_MEMORY_HOST_COHERENT);
    if (compareFlagBit(flags, vk::MemoryPropertyFlagBits::eHostCached))      flagsVector.push_back(STRING_VK_MEMORY_HOST_CACHED);
    if (compareFlagBit(flags, vk::MemoryPropertyFlagBits::eLazilyAllocated)) flagsVector.push_back(STRING_VK_MEMORY_LAZILY_ALLOCATED);

    return flagsVector;
}

std::vector<std::string> bufferUsageFlagsToVectorString(const vk::BufferUsageFlags flags) {

    auto flagsVector = std::vector<std::string> {};

    if (compareFlagBit(flags, vk::BufferUsageFlagBits::eTransferSrc))        flagsVector.push_back(STRING_VK_BUFFER_USAGE_TRANSFER_SRC);
    if (compareFlagBit(flags, vk::BufferUsageFlagBits::eTransferDst))        flagsVector.push_back(STRING_VK_BUFFER_USAGE_TRANSFER_DST);
    if (compareFlagBit(flags, vk::BufferUsageFlagBits::eUniformTexelBuffer)) flagsVector.push_back(STRING_VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER);
    if (compareFlagBit(flags, vk::BufferUsageFlagBits::eStorageTexelBuffer)) flagsVector.push_back(STRING_VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER);
    if (compareFlagBit(flags, vk::BufferUsageFlagBits::eUniformBuffer))      flagsVector.push_back(STRING_VK_BUFFER_USAGE_UNIFORM_BUFFER);
    if (compareFlagBit(flags, vk::BufferUsageFlagBits::eStorageBuffer))      flagsVector.push_back(STRING_VK_BUFFER_USAGE_STORAGE_BUFFER);
    if (compareFlagBit(flags, vk::BufferUsageFlagBits::eIndexBuffer))        flagsVector.push_back(STRING_VK_BUFFER_USAGE_INDEX_BUFFER);
    if (compareFlagBit(flags, vk::BufferUsageFlagBits::eVertexBuffer))       flagsVector.push_back(STRING_VK_BUFFER_USAGE_VERTEX_BUFFER);
    if (compareFlagBit(flags, vk::BufferUsageFlagBits::eIndirectBuffer))     flagsVector.push_back(STRING_VK_BUFFER_USAGE_INDIRECT_BUFFER);
    
    return flagsVector;
}

} // namespace ll
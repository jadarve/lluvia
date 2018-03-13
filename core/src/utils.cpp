#include "lluvia/core/utils.h"


namespace ll {


constexpr const char* STRING_VK_MEMORY_DEVICE_LOCAL     = "DEVICE_LOCAL";
constexpr const char* STRING_VK_MEMORY_HOST_CACHED      = "HOST_CACHED";
constexpr const char* STRING_VK_MEMORY_HOST_COHERENT    = "HOST_COHERENT";
constexpr const char* STRING_VK_MEMORY_HOST_VISIBLE     = "HOST_VISIBLE";
constexpr const char* STRING_VK_MEMORY_LAZILY_ALLOCATED = "LAZILY_ALLOCATED";


constexpr const char* STRING_VK_BUFFER_USAGE_INDEX_BUFFER         = "INDEX_BUFFER";
constexpr const char* STRING_VK_BUFFER_USAGE_INDIRECT_BUFFER      = "INDIRECT_BUFFE";
constexpr const char* STRING_VK_BUFFER_USAGE_STORAGE_BUFFER       = "STORAGE_BUFFER";
constexpr const char* STRING_VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER = "STORAGE_TEXEL_BUFFER";
constexpr const char* STRING_VK_BUFFER_USAGE_TRANSFER_DST         = "TRANSFER_DST";
constexpr const char* STRING_VK_BUFFER_USAGE_TRANSFER_SRC         = "TRANSFER_SRC";
constexpr const char* STRING_VK_BUFFER_USAGE_UNIFORM_BUFFER       = "UNIFORM_BUFFER";
constexpr const char* STRING_VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER = "UNIFORM_TEXEL_BUFFER";
constexpr const char* STRING_VK_BUFFER_USAGE_VERTEX_BUFFER        = "VERTEX_BUFFER";


constexpr const char* STRING_VK_IMAGE_USAGE_TRANSFER_SRC_BIT             = "IMAGE_USAGE_TRANSFER_SRC";
constexpr const char* STRING_VK_IMAGE_USAGE_TRANSFER_DST_BIT             = "IMAGE_USAGE_TRANSFER_DST";
constexpr const char* STRING_VK_IMAGE_USAGE_SAMPLED_BIT                  = "IMAGE_USAGE_SAMPLED";
constexpr const char* STRING_VK_IMAGE_USAGE_STORAGE_BIT                  = "IMAGE_USAGE_STORAGE";
constexpr const char* STRING_VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT         = "IMAGE_USAGE_COLOR_ATTACHMENT";
constexpr const char* STRING_VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT = "IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT";
constexpr const char* STRING_VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT     = "IMAGE_USAGE_TRANSIENT_ATTACHMENT";
constexpr const char* STRING_VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT         = "IMAGE_USAGE_INPUT_ATTACHMENT";


constexpr const auto BASE_64_ALPHABET = std::array<char, 65> {{'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/','='}};


auto compareFlagBit = [](const auto& flags, const auto& value) {
    return ((flags & value) == value);
};


vk::MemoryPropertyFlags vectorStringToMemoryPropertyFlags(const std::vector<std::string>& flagsVector) {

    auto flags = vk::MemoryPropertyFlags {};

    for (const auto& strFlag : flagsVector) {

        if (strFlag == STRING_VK_MEMORY_DEVICE_LOCAL)     flags |= vk::MemoryPropertyFlagBits::eDeviceLocal;
        if (strFlag == STRING_VK_MEMORY_HOST_CACHED)      flags |= vk::MemoryPropertyFlagBits::eHostCached;
        if (strFlag == STRING_VK_MEMORY_HOST_COHERENT)    flags |= vk::MemoryPropertyFlagBits::eHostCoherent;
        if (strFlag == STRING_VK_MEMORY_HOST_VISIBLE)     flags |= vk::MemoryPropertyFlagBits::eHostVisible;
        if (strFlag == STRING_VK_MEMORY_LAZILY_ALLOCATED) flags |= vk::MemoryPropertyFlagBits::eLazilyAllocated;
    }

    return flags;
}


std::vector<std::string> memoryPropertyFlagsToVectorString(const vk::MemoryPropertyFlags flags) {


    auto flagsVector = std::vector<std::string> {};

    if (compareFlagBit(flags, vk::MemoryPropertyFlagBits::eDeviceLocal))     flagsVector.push_back(STRING_VK_MEMORY_DEVICE_LOCAL);
    if (compareFlagBit(flags, vk::MemoryPropertyFlagBits::eHostCached))      flagsVector.push_back(STRING_VK_MEMORY_HOST_CACHED);
    if (compareFlagBit(flags, vk::MemoryPropertyFlagBits::eHostCoherent))    flagsVector.push_back(STRING_VK_MEMORY_HOST_COHERENT);
    if (compareFlagBit(flags, vk::MemoryPropertyFlagBits::eHostVisible))     flagsVector.push_back(STRING_VK_MEMORY_HOST_VISIBLE);
    if (compareFlagBit(flags, vk::MemoryPropertyFlagBits::eLazilyAllocated)) flagsVector.push_back(STRING_VK_MEMORY_LAZILY_ALLOCATED);

    return flagsVector;
}


vk::BufferUsageFlags vectorStringToBufferUsageFLags(const std::vector<std::string>& flagsVector) {

    auto flags = vk::BufferUsageFlags {};

    for (const auto& strFlag : flagsVector) {

        if (strFlag == STRING_VK_BUFFER_USAGE_INDEX_BUFFER)         flags |= vk::BufferUsageFlagBits::eIndexBuffer;
        if (strFlag == STRING_VK_BUFFER_USAGE_INDIRECT_BUFFER)      flags |= vk::BufferUsageFlagBits::eIndirectBuffer;
        if (strFlag == STRING_VK_BUFFER_USAGE_STORAGE_BUFFER)       flags |= vk::BufferUsageFlagBits::eStorageBuffer;
        if (strFlag == STRING_VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER) flags |= vk::BufferUsageFlagBits::eStorageTexelBuffer;
        if (strFlag == STRING_VK_BUFFER_USAGE_TRANSFER_DST)         flags |= vk::BufferUsageFlagBits::eTransferDst;
        if (strFlag == STRING_VK_BUFFER_USAGE_TRANSFER_SRC)         flags |= vk::BufferUsageFlagBits::eTransferSrc;
        if (strFlag == STRING_VK_BUFFER_USAGE_UNIFORM_BUFFER)       flags |= vk::BufferUsageFlagBits::eUniformBuffer;
        if (strFlag == STRING_VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER) flags |= vk::BufferUsageFlagBits::eUniformTexelBuffer;
        if (strFlag == STRING_VK_BUFFER_USAGE_VERTEX_BUFFER)        flags |= vk::BufferUsageFlagBits::eVertexBuffer;
    }

    return flags;
}


std::vector<std::string> bufferUsageFlagsToVectorString(const vk::BufferUsageFlags flags) {

    auto flagsVector = std::vector<std::string> {};

    if (compareFlagBit(flags, vk::BufferUsageFlagBits::eIndexBuffer))        flagsVector.push_back(STRING_VK_BUFFER_USAGE_INDEX_BUFFER);
    if (compareFlagBit(flags, vk::BufferUsageFlagBits::eIndirectBuffer))     flagsVector.push_back(STRING_VK_BUFFER_USAGE_INDIRECT_BUFFER);
    if (compareFlagBit(flags, vk::BufferUsageFlagBits::eStorageBuffer))      flagsVector.push_back(STRING_VK_BUFFER_USAGE_STORAGE_BUFFER);
    if (compareFlagBit(flags, vk::BufferUsageFlagBits::eStorageTexelBuffer)) flagsVector.push_back(STRING_VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER);
    if (compareFlagBit(flags, vk::BufferUsageFlagBits::eTransferDst))        flagsVector.push_back(STRING_VK_BUFFER_USAGE_TRANSFER_DST);
    if (compareFlagBit(flags, vk::BufferUsageFlagBits::eTransferSrc))        flagsVector.push_back(STRING_VK_BUFFER_USAGE_TRANSFER_SRC);
    if (compareFlagBit(flags, vk::BufferUsageFlagBits::eUniformBuffer))      flagsVector.push_back(STRING_VK_BUFFER_USAGE_UNIFORM_BUFFER);
    if (compareFlagBit(flags, vk::BufferUsageFlagBits::eUniformTexelBuffer)) flagsVector.push_back(STRING_VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER);
    if (compareFlagBit(flags, vk::BufferUsageFlagBits::eVertexBuffer))       flagsVector.push_back(STRING_VK_BUFFER_USAGE_VERTEX_BUFFER);
    
    return flagsVector;
}


vk::ImageUsageFlags vectorStringToImageUsageFlags(const std::vector<std::string>& flagsVector) {

    auto flags = vk::ImageUsageFlags {};

    for (const auto& strFlag : flagsVector) {

        if (strFlag == STRING_VK_IMAGE_USAGE_TRANSFER_SRC_BIT)             flags |= vk::ImageUsageFlagBits::eTransferSrc;
        if (strFlag == STRING_VK_IMAGE_USAGE_TRANSFER_DST_BIT)             flags |= vk::ImageUsageFlagBits::eTransferDst;
        if (strFlag == STRING_VK_IMAGE_USAGE_SAMPLED_BIT)                  flags |= vk::ImageUsageFlagBits::eSampled;
        if (strFlag == STRING_VK_IMAGE_USAGE_STORAGE_BIT)                  flags |= vk::ImageUsageFlagBits::eStorage;
        if (strFlag == STRING_VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)         flags |= vk::ImageUsageFlagBits::eColorAttachment;
        if (strFlag == STRING_VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) flags |= vk::ImageUsageFlagBits::eDepthStencilAttachment;
        if (strFlag == STRING_VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT)     flags |= vk::ImageUsageFlagBits::eTransientAttachment;
        if (strFlag == STRING_VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT)         flags |= vk::ImageUsageFlagBits::eInputAttachment;
    }

    return flags;
}


std::vector<std::string> ImageUsageFlagsToVectorString(const vk::ImageUsageFlags flags) {

    auto flagsVector = std::vector<std::string> {};

    if (compareFlagBit(flags, vk::ImageUsageFlagBits::eTransferSrc))            flagsVector.push_back(STRING_VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
    if (compareFlagBit(flags, vk::ImageUsageFlagBits::eTransferDst))            flagsVector.push_back(STRING_VK_IMAGE_USAGE_TRANSFER_DST_BIT);
    if (compareFlagBit(flags, vk::ImageUsageFlagBits::eSampled))                flagsVector.push_back(STRING_VK_IMAGE_USAGE_SAMPLED_BIT);
    if (compareFlagBit(flags, vk::ImageUsageFlagBits::eStorage))                flagsVector.push_back(STRING_VK_IMAGE_USAGE_STORAGE_BIT);
    if (compareFlagBit(flags, vk::ImageUsageFlagBits::eColorAttachment))        flagsVector.push_back(STRING_VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
    if (compareFlagBit(flags, vk::ImageUsageFlagBits::eDepthStencilAttachment)) flagsVector.push_back(STRING_VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
    if (compareFlagBit(flags, vk::ImageUsageFlagBits::eTransientAttachment))    flagsVector.push_back(STRING_VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT);
    if (compareFlagBit(flags, vk::ImageUsageFlagBits::eInputAttachment))        flagsVector.push_back(STRING_VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT);

    return flagsVector;
}


std::string toBase64(const void* ptr, const size_t size) {

    const uint8_t* data = static_cast<const uint8_t*>(ptr);

    const auto modulus = size % 3;
    const auto N       = size - modulus;

    // size of the encoded array
    const auto encodedSize = 4*(N/3) + (modulus == 0? 0 : 4);
    auto code = std::string(encodedSize, '*');

    // encode the first N bytes of data
    auto word = uint32_t {0u};
    
    // encode the first N bytes of data
    auto c = -1;
    for (auto i = 0u; i < N; i += 3) {

        // 3 bytes word to be encoded
        word = (static_cast<uint32_t>(data[i])       << 16) |
               (static_cast<uint32_t>(data[i + 1])   <<  8) |
               (static_cast<uint32_t>(data[i + 2]));

        code[++c] = BASE_64_ALPHABET[(word & 0xFC0000) >> 18];
        code[++c] = BASE_64_ALPHABET[(word & 0x03F000) >> 12];
        code[++c] = BASE_64_ALPHABET[(word & 0x000FC0) >> 6];
        code[++c] = BASE_64_ALPHABET[(word & 0x00003F)];
    }

    // encodes the last bytes of buffer
    word = 0u;
    switch (modulus) {

    case 0:
        // nothing to do
        break;

    case 1:
        word = (static_cast<uint32_t>(data[N])       << 16);

        code[++c] = BASE_64_ALPHABET[(word & 0xFC0000) >> 18];
        code[++c] = BASE_64_ALPHABET[(word & 0x03F000) >> 12];
        code[++c] = BASE_64_ALPHABET[64];     // padding
        code[++c] = BASE_64_ALPHABET[64];     // padding
        break;

    default: // 2
        word = (static_cast<uint32_t>(data[N])       << 16) |
               (static_cast<uint32_t>(data[N + 1])   <<  8);

        code[++c] = BASE_64_ALPHABET[(word & 0xFC0000) >> 18];
        code[++c] = BASE_64_ALPHABET[(word & 0x03F000) >> 12];
        code[++c] = BASE_64_ALPHABET[(word & 0x000FC0) >> 6];
        code[++c] = BASE_64_ALPHABET[64];     // padding       
        break;
    }

    return code;
}


std::vector<uint8_t> fromBase64(const std::string& code) {

    auto lookUp = [](const char& c) {

        if (c >='A' && c <='Z') return static_cast<uint32_t>(static_cast<uint32_t>(c) - static_cast<uint32_t>('A'));
        if (c >='a' && c <='z') return static_cast<uint32_t>(static_cast<uint32_t>(c) - 71);
        if (c >='0' && c <='9') return static_cast<uint32_t>(static_cast<uint32_t>(c) + 4);
        if (c == '+') return uint32_t {62u};
        if (c == '/') return uint32_t {63u};

        return static_cast<uint32_t>('=');
    };

    const auto size = code.size();

    auto padding = code[size - 2] == '='? 2u : code[size - 1] == '='? 1u : 0u;
    auto N = (3*(size - padding)) / 4;

    auto binary = std::vector<uint8_t>(N);

    auto n = -1;
    for (auto i = 0u; i < size; i += 4) {

        const auto w0 = lookUp(code[i]);
        const auto w1 = lookUp(code[i + 1]);
        const auto w2 = lookUp(code[i + 2]);
        const auto w3 = lookUp(code[i + 3]);

        binary[++n] = static_cast<uint8_t>((w0 << 2) | ((w1 & 0x30) >> 4));
        binary[++n] = static_cast<uint8_t>(((w1 & 0x0F) << 4) | ((w2 & 0x3C) >> 2));
        binary[++n] = static_cast<uint8_t>(((w2 & 0x03) << 6) | w3);
    }

    return binary;
}

} // namespace ll

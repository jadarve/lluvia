/**
@file       io.cpp
@brief      utility functions.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#include "lluvia/core/utils.h"

#include "lluvia/core/CommandBuffer.h"
#include "lluvia/core/Session.h"
#include "lluvia/core/image/Image.h"
#include "lluvia/core/image/ImageDescriptor.h"
#include "lluvia/core/memory/Memory.h"

#include <algorithm>
#include <cmath>

namespace ll {

constexpr const auto BASE_64_ALPHABET = std::array<char, 65> {{'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '+', '/',
    '='}};

std::string toBase64(const void* ptr, const size_t size)
{

    const uint8_t* data = static_cast<const uint8_t*>(ptr);

    const auto modulus = size % 3;
    const auto N       = size - modulus;

    // size of the encoded array
    const auto encodedSize = 4 * (N / 3) + (modulus == 0 ? 0 : 4);
    auto       code        = std::string(encodedSize, '*');

    // encode the first N bytes of data
    auto word = uint32_t {0u};

    // encode the first N bytes of data
    auto c = -1;
    for (auto i = 0u; i < N; i += 3) {

        // 3 bytes word to be encoded
        word = (static_cast<uint32_t>(data[i]) << 16) | (static_cast<uint32_t>(data[i + 1]) << 8) | (static_cast<uint32_t>(data[i + 2]));

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
        word = (static_cast<uint32_t>(data[N]) << 16);

        code[++c] = BASE_64_ALPHABET[(word & 0xFC0000) >> 18];
        code[++c] = BASE_64_ALPHABET[(word & 0x03F000) >> 12];
        code[++c] = BASE_64_ALPHABET[64]; // padding
        code[++c] = BASE_64_ALPHABET[64]; // padding
        break;

    default: // 2
        word = (static_cast<uint32_t>(data[N]) << 16) | (static_cast<uint32_t>(data[N + 1]) << 8);

        code[++c] = BASE_64_ALPHABET[(word & 0xFC0000) >> 18];
        code[++c] = BASE_64_ALPHABET[(word & 0x03F000) >> 12];
        code[++c] = BASE_64_ALPHABET[(word & 0x000FC0) >> 6];
        code[++c] = BASE_64_ALPHABET[64]; // padding
        break;
    }

    return code;
}

std::vector<uint8_t> fromBase64(const std::string& code)
{

    auto lookUp = [](const char& c) {
        if (c >= 'A' && c <= 'Z')
            return static_cast<uint32_t>(static_cast<uint32_t>(c) - static_cast<uint32_t>('A'));
        if (c >= 'a' && c <= 'z')
            return static_cast<uint32_t>(static_cast<uint32_t>(c) - 71);
        if (c >= '0' && c <= '9')
            return static_cast<uint32_t>(static_cast<uint32_t>(c) + 4);
        if (c == '+')
            return uint32_t {62u};
        if (c == '/')
            return uint32_t {63u};
        if (c == '=')
            return static_cast<uint32_t>('=');

        throw std::runtime_error {"unknown character for base 64 parsing: " + std::string {c}};
    };

    const auto size = code.size();

    auto padding = code[size - 2] == '=' ? 2u : code[size - 1] == '=' ? 1u
                                                                      : 0u;
    auto N       = (3 * (size - padding)) / 4;

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

std::shared_ptr<ll::Image> createAndInitImage(
    std::shared_ptr<ll::Session>& session,
    std::shared_ptr<ll::Memory>&  memory,
    const ll::ImageDescriptor&    desc,
    const ll::ImageLayout         initialLayout)
{

    std::shared_ptr<ll::Image> image = memory->createImage(desc);

    {
        auto cmdBuffer = session->createCommandBuffer();

        cmdBuffer->begin();
        cmdBuffer->changeImageLayout(*image, initialLayout);
        cmdBuffer->end();

        session->run(*cmdBuffer);
    }

    return image;
}

std::shared_ptr<ll::ImageView> createAndInitImageView(
    std::shared_ptr<ll::Session>&  session,
    std::shared_ptr<ll::Memory>&   memory,
    const ll::ImageDescriptor&     imgDescriptor,
    const ll::ImageViewDescriptor& viewDescriptor,
    const ll::ImageLayout          initialLayout)
{

    auto image = createAndInitImage(session, memory, imgDescriptor, initialLayout);

    return image->createImageView(viewDescriptor);
}

} // namespace ll

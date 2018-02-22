#ifndef LLUVIA_CORE_IMAGE_H_
#define LLUVIA_CORE_IMAGE_H_

#include "lluvia/core/ImageDescriptor.h"
#include "lluvia/core/MemoryAllocationInfo.h"
#include "lluvia/core/Object.h"


#include <array>
#include <cstdint>

#include <vulkan/vulkan.hpp>


namespace ll {

class Memory;


class Image: public Object {

public:
    Image()                = delete;
    Image(const Image&)    = delete;
    Image(Image&&)         = delete;

    ~Image();

    Image& operator = (const Image&) = delete;
    Image& operator = (Image&&)      = delete;

    ObjectType getType() const noexcept override;

private:
    ll::ImageDescriptor descriptor;
    ll::MemoryAllocationInfo allocInfo;

    uint64_t rowPitch;

    // Shared pointer to the memory this image was created from
    // This will keep the memory alive until this image is deleted
    // avoiding reference to a corrupted memory location.
    std::shared_ptr<ll::Memory> memory;
};

} // namespace ll

#endif /* LLUVIA_CORE_IMAGE_H_ */

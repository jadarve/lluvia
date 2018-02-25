#include "lluvia/core/Image.h"

#include "lluvia/core/Memory.h"

namespace ll {

Image::~Image() {

}


ObjectType Image::getType() const noexcept {
    return ObjectType::Image;
}

} // namespace ll

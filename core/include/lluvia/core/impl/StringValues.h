#ifndef LLUVIA_CORE_IMPL_STRING_VALUES_H_
#define LLUVIA_CORE_IMPL_STRING_VALUES_H_

#include <array>

namespace ll {
namespace impl {

constexpr const std::array<const char*, 3> ObjectTypeStrings {{
    "BUFFER",
    "IMAGE",
    "IMAGE_VIEW"
}};

} // namespace impl
} // namespace ll

#endif // LLUVIA_CORE_IMPL_STRING_VALUES_H_

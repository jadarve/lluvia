#ifndef LLUVIA_CORE_OBJECT_H_
#define LLUVIA_CORE_OBJECT_H_

#include "lluvia/core/impl/enum_utils.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <stdexcept>
#include <string>

namespace ll {

enum class ObjectType : uint32_t {
    Buffer     = 0,
    Image      = 1,
    ImageView  = 2
};


namespace impl {

    constexpr const std::array<const char*, 3> ObjectTypeStrings {{
        "BUFFER",
        "IMAGE",
        "IMAGE_VIEW"
    }};

} // namespace impl


template<typename T = std::string>
inline T objectTypeToString(ll::ObjectType&& value) {
    return impl::enumToString<ll::ObjectType, ll::impl::ObjectTypeStrings.size(), ll::impl::ObjectTypeStrings>(std::forward<ll::ObjectType>(value));
}


template<typename T>
inline ll::ObjectType stringToObjectType(T&& stringValue) {
    return impl::stringToEnum<ll::ObjectType, T, ll::impl::ObjectTypeStrings.size(), ll::impl::ObjectTypeStrings>(std::forward<T>(stringValue));
}


class Object {

public:
    Object()              = default;
    Object(const Object&) = default;
    Object(Object&&)      = default;

    virtual ~Object()     = default;

    Object& operator = (const Object&) = default;
    Object& operator = (Object&&)      = default;

    virtual ll::ObjectType getType() const noexcept = 0;
};


} // namespace ll

#endif // LLUVIA_CORE_OBJECT_H_

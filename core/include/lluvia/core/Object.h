#ifndef LLUVIA_CORE_OBJECT_H_
#define LLUVIA_CORE_OBJECT_H_

#include "lluvia/core/impl/StringValues.h"

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


template<typename T = std::string>
T objectTypeToString(const ObjectType type) {
    return impl::ObjectTypeStrings[static_cast<uint32_t>(type)];
}


template<typename T>
ll::ObjectType stringToObjectType(T&& name) {
    static_assert(std::is_convertible<T, std::string>(), "T must be a string-like type");

    auto it = std::find(impl::ObjectTypeStrings.cbegin(), impl::ObjectTypeStrings.cend(), name);
    if (it == impl::ObjectTypeStrings.cend()) throw std::out_of_range("invalid name for object type: " + name);

    return static_cast<ll::ObjectType>(std::distance(impl::ObjectTypeStrings.cbegin(), it));
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

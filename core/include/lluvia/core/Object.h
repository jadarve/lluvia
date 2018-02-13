#ifndef LLUVIA_CORE_OBJECT_H_
#define LLUVIA_CORE_OBJECT_H_

#include <cstdint>
#include <string>

namespace ll {


enum class ObjectType : uint32_t {
    Buffer
};


std::string objectTypeToString(const ObjectType type);


class Object {

public:
    Object()              = default;
    Object(const Object&) = default;
    Object(Object&&)      = default;

    virtual ~Object()     = default;

    Object& operator = (const Object&) = default;
    Object& operator = (Object&&)      = default;

    virtual ObjectType getType() const noexcept = 0;
};


} // namespace ll

#endif // LLUVIA_CORE_OBJECT_H_

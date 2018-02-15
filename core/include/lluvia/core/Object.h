#ifndef LLUVIA_CORE_OBJECT_H_
#define LLUVIA_CORE_OBJECT_H_

#include <cstdint>
#include <stdexcept>
#include <string>

namespace ll {


enum class ObjectType : uint32_t {
    Buffer
};


std::string objectTypeToString(const ObjectType type);

template<typename T>
ll::ObjectType stringToObjectType(T&& name) {

    // FIXME: check that T is a string-like type 

    if (name == "buffer") {return ll::ObjectType::Buffer;}

    throw std::out_of_range("invalid name for object type: " + name);
}


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

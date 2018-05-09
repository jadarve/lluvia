/**
@file       Object.h
@brief      Object class and related enums.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_OBJECT_H_
#define LLUVIA_CORE_OBJECT_H_

#include "lluvia/core/impl/enum_utils.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <tuple>

namespace ll {

/**
@brief      Object types.

Each value represent one of the ll::Object child classes.

@sa ll::impl::ObjectTypeStrings string values for this enum.
*/
enum class ObjectType : uint32_t {
    Buffer     = 0,     /**< value for ll::Buffer. */
    Image      = 1,     /**< value for ll::Image. */
    ImageView  = 2      /**< value for ll::ImageView. */
};


namespace impl {

    /**
    @brief Object type string values used for converting ll::ObjectType to std::string and vice-versa.

    @sa ll::ObjectType enum values for this array.
    */
    constexpr const std::array<std::tuple<const char*, ll::ObjectType>, 3> ObjectTypeStrings2 {{
        {"BUFFER"     , ll::ObjectType::Buffer},
        {"IMAGE"      , ll::ObjectType::Image},
        {"IMAGE_VIEW" , ll::ObjectType::ImageView},
    }};

} // namespace impl


/**
@brief      Converts from ll::ObjectType enum value to std::string.

@param[in]  value      

@tparam     T          function return type. Defaults to std::string.

@return     Returns the corresponding `std::string` in ll::impl::ObjectTypeStrings for the enum value.
*/
template<typename T = std::string>
inline T objectTypeToString(ll::ObjectType&& value) noexcept {
    return impl::enumToString<ll::ObjectType, ll::impl::ObjectTypeStrings2.size(), ll::impl::ObjectTypeStrings2>(std::forward<ll::ObjectType>(value));
}


/**
@brief      Converts from a string-like object to ll::ObjectType enum.

This function can be used either with string literals, const char* or `std::string` objects.
\p stringValue parameter is compared against the values in ll::impl::ObjectTypeStrings and the
corresponding enum value is returned. The comparison is case sensitive.

@param[in]  stringValue  string-like parameter. String literals and `std::string` objects are allowed.

@tparam     T            \p stringValue type. \p T must satisfies `std::is_convertible<T, std::string>()`

@return     ll::ObjectType value corresponding to stringValue
 
@throws std::out_of_range if \p stringValue is not found in ll::impl::ObjectTypeStrings.
*/
template<typename T>
inline ll::ObjectType stringToObjectType(T&& stringValue) {
    return impl::stringToEnum<ll::ObjectType, T, ll::impl::ObjectTypeStrings2.size(), ll::impl::ObjectTypeStrings2>(std::forward<T>(stringValue));
}


/**
@brief Base class for all types that can be used in computer shaders.

Handling of objects in the engine is done through instances allocated in the heap and referenced
through pointers. The reason is that normal object operations such as copying or moving are hard
to define for classes that contain some Vulkan resource such as buffers or images.
 
*/
class Object {

public:
    /**
    @brief      Default constructor.

    This constructor needs to be kept in order to construct the child classes. However, this class
    has no members to initialize.
    */
    Object()              = default;


    /**
    @brief      Copy constructor.

    The copy constructor is deleted intentionally.
    Copying objects that handle some Vulkan resource
    such as buffers or images is not well defined.
    */
    Object(const Object&) = delete;


    /**
    @brief      Move constructor.

    The move constructor is deleted intentionally.
    Moving objects that handle Vulkan resources is not well defined.
    */
    Object(Object&&)      = delete;


    virtual ~Object()     = default;

    /**
    @brief      Copy assignment.

    Copy assignment is not allowed for Object instances.
    */
    Object& operator = (const Object&) = delete;


    /**
    @brief      Move assignment.

    Move assignment is not allowed for Object instances.
    */
    Object& operator = (Object&&)      = delete;


    /**
    @brief      Gets the object type.

    This method can be used for safely down-casting pointers of ll::Object type
    to its children.

    @code
    ll::Object* ptr = ...

    if (ptr->getType() == ll::ObjectType::Buffer) {
        auto bufferPtr = static_cast<ll::Buffer*>(ptr);
    }
    @endcode
    
    @return     The object type.
    */
    virtual ll::ObjectType getType() const noexcept = 0;
};


} // namespace ll

#endif // LLUVIA_CORE_OBJECT_H_

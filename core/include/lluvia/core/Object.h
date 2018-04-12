#ifndef LLUVIA_CORE_OBJECT_H_
#define LLUVIA_CORE_OBJECT_H_

#include "lluvia/core/impl/enum_utils.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <stdexcept>
#include <string>

namespace ll {

/**
 * @brief      Object types.
 */
enum class ObjectType : uint32_t {
    Buffer     = 0,     /**< value for ll::Buffer. */
    Image      = 1,     /**< value for ll::Image. */
    ImageView  = 2      /**< value for ll::ImageView. */
};


namespace impl {

    /**
     * Object type string values used for converting ll::ObjectType to std::string and vice-versa.
     */
    constexpr const std::array<const char*, 3> ObjectTypeStrings {{
        "BUFFER",
        "IMAGE",
        "IMAGE_VIEW"
    }};

} // namespace impl


/**
 * @brief      Converts from ll::ObjectType enum value to std::string.
 *
 * @param[in]  value      
 *
 * @tparam     T          function return type. Defaults to std::string.
 *
 * @return     Returns the corresponding `std::string` in ll::impl::ObjectTypeStrings for the enum value.
 */
template<typename T = std::string>
inline T objectTypeToString(ll::ObjectType&& value) {
    return impl::enumToString<ll::ObjectType, ll::impl::ObjectTypeStrings.size(), ll::impl::ObjectTypeStrings>(std::forward<ll::ObjectType>(value));
}


/**
 * @brief      Converts from a string-like object to ll::ObjectType enum.
 *
 * This function can be used either with string literas, const char* or `std::string` objects.
 * \p stringValue parameter is compared against the values in ll::ObjectTypeStrings and the
 * corresponding enum value is returned. The comparison is case sensitive.
 *
 * @param[in]  stringValue  string-like parameter. String literals and `std::string` objects are allowes.
 *
 * @tparam     T            \p stringValue type. \p T must satisfies `std::is_convertible<T, std::string>()`
 *
 * @return     ll::ObjectType value corresponding to stringValue
 * 
 * @throws std::out_of_range if \p stringValue is not found in ll::ObjectTypeStrings.
 */
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

#ifndef LLUVIA_CORE_IMPL_ENUM_UTILS_H_
#define LLUVIA_CORE_IMPL_ENUM_UTILS_H_

#include <algorithm>
#include <array>
#include <cstdint>
#include <string>
#include <typeinfo>



namespace ll {
namespace impl {


template<typename T, std::size_t N, const std::array<const char*, N>& stringValues>
inline std::string enumToString(const T value) {
    return stringValues.at(static_cast<uint32_t>(value));
}


template<typename E, typename T, std::size_t N, const std::array<const char*, N>& stringValues>
inline E stringToEnum(T&& name) {

    static_assert(std::is_convertible<T, std::string>(), "T must be a string-like type");

    auto it = std::find(stringValues.cbegin(), stringValues.cend(), name);

    if (it == stringValues.cend()) {
        throw std::out_of_range("invalid string value [" + name + "] for enum type " + typeid(T).name());
    }

    return static_cast<E>(std::distance(stringValues.cbegin(), it));
}


} // namespace impl
} // namespace ll

#endif // LLUVIA_CORE_IMPL_ENUM_UTILS_H_

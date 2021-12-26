/**
@file       enums.h
@brief      Enums.
@copyright  2021, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_ENUMS_H_
#define LLUVIA_CORE_ENUMS_H_

#include <algorithm>
#include <array>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <tuple>
#include <typeinfo>
#include <vector>

// #include "lluvia/core/vulkan/vulkan.hpp"

#define LL_CONSTEXPR constexpr

namespace ll {

using enum_t = uint32_t;


template<typename BitType, typename MaskType = enum_t>
class Flags {

public:
    Flags() = default;
    Flags(const Flags<BitType, MaskType>& f) = default;
    Flags(Flags<BitType, MaskType>&& f) = default;

    constexpr Flags(BitType bit) noexcept:
        m_mask{static_cast<MaskType>(bit)} {
    }

    constexpr explicit Flags(MaskType mask):
        m_mask {mask} {
    }

    ~Flags() = default;

    // relational operators
    LL_CONSTEXPR bool operator<(Flags<BitType, MaskType> const &rhs) const noexcept {
        return m_mask < rhs.m_mask;
    }

    LL_CONSTEXPR bool operator<=(Flags<BitType, MaskType> const &rhs) const noexcept {
        return m_mask <= rhs.m_mask;
    }

    LL_CONSTEXPR bool operator>(Flags<BitType, MaskType> const &rhs) const noexcept {
        return m_mask > rhs.m_mask;
    }

    LL_CONSTEXPR bool operator>=(Flags<BitType, MaskType> const &rhs) const noexcept {
        return m_mask >= rhs.m_mask;
    }

    LL_CONSTEXPR bool operator==(Flags<BitType, MaskType> const &rhs) const noexcept {
        return m_mask == rhs.m_mask;
    }

    LL_CONSTEXPR bool operator!=(Flags<BitType, MaskType> const &rhs) const noexcept {
        return m_mask != rhs.m_mask;
    }

    // logical operator
    LL_CONSTEXPR bool operator!() const noexcept {
        return !m_mask;
    }

    // bitwise operators
    LL_CONSTEXPR Flags<BitType, MaskType> operator&(Flags<BitType, MaskType> const &rhs) const noexcept {
        return Flags<BitType, MaskType>(m_mask & rhs.m_mask);
    }

    LL_CONSTEXPR Flags<BitType, MaskType> operator|(Flags<BitType, MaskType> const &rhs) const noexcept {
        return Flags<BitType, MaskType>(m_mask | rhs.m_mask);
    }

    LL_CONSTEXPR Flags<BitType, MaskType> operator^(Flags<BitType, MaskType> const &rhs) const noexcept {
        return Flags<BitType, MaskType>(m_mask ^ rhs.m_mask);
    }

    // LL_CONSTEXPR Flags<BitType, MaskType> operator~() const noexcept {
    //     return Flags<BitType, MaskType>(m_mask ^ FlagTraits<BitType>::allFlags);
    // }

    // assignment operators
    Flags<BitType, MaskType> &operator=(Flags<BitType, MaskType> const &rhs) noexcept {
        m_mask = rhs.m_mask;
        return *this;
    }

    Flags<BitType, MaskType> &operator|=(Flags<BitType, MaskType> const &rhs) noexcept {
        m_mask |= rhs.m_mask;
        return *this;
    }

    Flags<BitType, MaskType> &operator&=(Flags<BitType, MaskType> const &rhs) noexcept {
        m_mask &= rhs.m_mask;
        return *this;
    }

    Flags<BitType, MaskType> &operator^=(Flags<BitType, MaskType> const &rhs) noexcept {
        m_mask ^= rhs.m_mask;
        return *this;
    }

    // cast operators
    explicit LL_CONSTEXPR operator bool() const noexcept {
        return !!m_mask;
    }

    explicit LL_CONSTEXPR operator MaskType() const noexcept {
        return m_mask;
    }

private:
    MaskType m_mask {0};
};


namespace impl {


template<typename E, std::size_t N, const std::array<std::tuple<const char*, E>, N>& values>
inline std::string enumToString(E&& value) noexcept {

    auto compare = [&value](const std::tuple<const char*, E>& e) {
        return std::get<1>(e) == value;
    };

    auto it = std::find_if(values.cbegin(), values.cend(), compare);

    return std::get<0>(*it);
}


template<typename E, typename T, std::size_t N, const std::array<std::tuple<const char*, E>, N>& values>
inline E stringToEnum(T&& stringValue) {

    static_assert(std::is_convertible<T, std::string>(), "T must be a string-like type");

    auto compare = [&stringValue](const std::tuple<const char*, E>& e) {
        return std::get<0>(e) == stringValue;
    };

    auto it = std::find_if(values.cbegin(), values.cend(), compare);

    if (it == values.cend()) {
        throw std::out_of_range("invalid string value [" + stringValue + "] for enum type " + typeid(E).name());
    }

    return std::get<1>(*it);
}


template<typename E, typename Ebits, std::size_t N, const std::array<std::tuple<const char*, Ebits>, N>& values>
inline E vectorStringToFlags(const std::vector<std::string>& flagsVector) noexcept {

	auto flags = E {};

	for (const auto& strFlag : flagsVector) {

		for (const auto& flagPair : values) {

			if (std::get<0>(flagPair) == strFlag) {
				flags |= std::get<1>(flagPair);
				break;
			}
		}
	}

	return flags;
}


template<typename E, typename Ebits, std::size_t N, const std::array<std::tuple<const char*, Ebits>, N>& values>
inline std::vector<std::string> flagsToVectorString(E flags) noexcept {

    auto flagsVector = std::vector<std::string> {};

    for (const auto& flagPair : values) {

        const auto& vFlags = std::get<1>(flagPair);

        if ((flags & vFlags) == vFlags) {
            flagsVector.push_back(std::get<0>(flagPair));
        }
    }

    return flagsVector;
}


} // namespace impl
} // namespace ll

#endif // LLUVIA_CORE_ENUMS_H_

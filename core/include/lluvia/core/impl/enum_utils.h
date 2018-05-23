/**
@file       enum_utils.h
@brief      Utility methods to manipulate enumerations.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_IMPL_ENUM_UTILS_H_
#define LLUVIA_CORE_IMPL_ENUM_UTILS_H_

#include <algorithm>
#include <array>
#include <cstdint>
#include <string>
#include <tuple>
#include <typeinfo>
#include <vector>


namespace ll {
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

#endif // LLUVIA_CORE_IMPL_ENUM_UTILS_H_
